#include "JellyMenuBetter.h"

#include "JellySplash.h"
#include "JellyGame.h"
#include "JellyOptions.h"

#include "../Utils/JellyHelper.h"
#include "../Utils/RectangleDrawer.h"
#include "../Utils/UiTheme.h"
#include "../Utils/UiLayout.h"
#include "../Utils/UiPrompt.h"

#include <Andromeda/Utils/Logger.h>

#include <stdio.h>
#include <cmath>

#define LOG(...) Andromeda::Utils::Logger::Instance()->Info(__FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)

JellyMenuBetter::JellyMenuBetter(JellyCore* core)
{
	_core = core;
}

void JellyMenuBetter::Init()
{
	LOG("JellyMenuBetter::Init begin\n");

	_renderManager = RenderManager::Instance();
	_shaderManager = ShaderManager::Instance();
	_textureManager = TextureManager::Instance();

	_renderManager->SetClearColor(0xffffffff);

	//font
	_menuFont = _core->_menuFont;
	_titleFont = _core->_titleFont;
	_smallFont = _core->_smallFont;

	//load shader
	_shader = _shaderManager->LoadFromFile("sprite", "Assets/Shaders/sprite", "Assets/Shaders/sprite", Textured);

	_projection = glm::ortho(0.0f, (float)_renderManager->GetWidth(), (float)_renderManager->GetHeight(), 0.0f, -1.0f, 1.0f);

	_renderManager->SetDepth(false);

	//input
	_inputHelper = InputHelper::Instance();

	//init audio helper
	_audioHelper = AudioHelper::Instance();
	_audioHelper->PlayMusic();

	//level manager
	_levelManager = new LevelManager();
	_levelManager->SetAssetsLocation("Assets/Jelly/");
	_levelManager->LoadAllScenes("scene_list.xml");
	_levelManager->LoadModLevels();  // inject mod levels after built-ins
	_levelManager->LoadCarSkins("car_skins.xml");
	_levelManager->LoadModSkins();   // inject mod skins after built-ins
	_levelManager->LoadScores("JellyScore.xml");

	//get scenes and skin names
	_sceneFiles = _levelManager->GetScenes();
	_carSkins   = _levelManager->GetCarSkins();

	LOG("JellyMenuBetter::Init scene count=%d skin count=%d\n", (int)_sceneFiles.size(), (int)_carSkins.size());

	//background + level thumbnail
	_backSprite = new Sprite("paper", "Assets/Jelly/Texture/paper.png", "Assets/Shaders/sprite", "Assets/Shaders/sprite");
	_levelImage = new Sprite("thumb", _levelManager->GetThumb(_sceneFiles[0]), _shader);

	_jellyGame = nullptr;
	_focusPulse = 0.0f;
	_focusPhase = 0.0f;

	//load menu level physics
	_world = new World();
	_levelManager->InitPhysic(_world);
	_levelManager->LoadCompiledLevel(_world, "menu.scene", "Assets/Jelly/car_and_truck.car");

	_car = _levelManager->GetCar();
	_car->SetChassisTextures(_levelManager->GetCarImage(_carSkins[0].chassisSmall), _levelManager->GetCarImage(_carSkins[0].chassisBig));
	_car->SetTireTextures(_levelManager->GetCarImage(_carSkins[0].tireSmall), _levelManager->GetCarImage(_carSkins[0].tireBig));

	_gameBodies = _levelManager->GetLevelBodies();

	_jellyProjection = glm::ortho(-20.0f, 20.0f, -4.2f + 4.0f, 4.0f + 18.2f, -1.0f, 1.0f);

	// --- grid state ---
	_gridCol     = 0;
	_gridRow     = 0;
	_currentPage = 0;
	_totalPages  = (int)ceil((float)_sceneFiles.size() / (float)PAGE_SIZE);
	if (_totalPages < 1) _totalPages = 1;

	LOG("JellyMenuBetter::Init grid pages=%d pageSize=%d\n", _totalPages, PAGE_SIZE);

	// car skin selection
	carcurrentPosition  = 0;
	carcolumnStartPosition = 0;
	carpositionsInColumn   = 5;

	//timer
	_dt    = 0;
	_timer = new Timer();

	LOG("JellyMenuBetter::Init done\n");
}

void JellyMenuBetter::Enter()
{
}

void JellyMenuBetter::CleanUp()
{
	LOG("JellyMenuBetter::CleanUp\n");

	if (_levelManager != 0)
	{
		_levelManager->ClearLevel(_world);
		delete _levelManager;
	}

	delete _world;
	_gameBodies.clear();

	delete _backSprite;
	delete _levelImage;

	_shaderManager->RemoveAll();
	_textureManager->RemoveAll();
}

void JellyMenuBetter::Pause()
{
}

void JellyMenuBetter::Resume()
{
	LOG("JellyMenuBetter::Resume\n");

	if (_jellyGame != nullptr)
	{
		delete _jellyGame;
		_jellyGame = nullptr;
	}

	_inputHelper->Update();
}

void JellyMenuBetter::GamePause()
{
}

void JellyMenuBetter::GameResume()
{
}

void JellyMenuBetter::HandleEvents(GameManager* manager)
{
	if (_inputHelper->MenuActionPressed(MenuAction::MenuExit))
	{
		LOG("JellyMenuBetter::HandleEvents exit\n");
		manager->Quit();
		return;
	}

	if (_inputHelper->MenuActionPressed(MenuAction::MenuPause))
	{
		LOG("JellyMenuBetter::HandleEvents open options\n");
		JellyOptions* jelly = new JellyOptions(_core);
		manager->PushState(jelly);
		return;
	}

	int totalLevels = (int)_sceneFiles.size();

	// --- grid navigation ---
	// Right: move col forward, wrap to next page
	if (_inputHelper->MenuActionPressed(MenuAction::MenuRight))
	{
		_gridCol++;
		if (_gridCol >= GRID_COLS)
		{
			_gridCol = 0;
			_currentPage++;
			if (_currentPage >= _totalPages)
				_currentPage = 0;
			LOG("JellyMenuBetter::HandleEvents page forward -> page=%d\n", _currentPage);
		}
		// clamp to valid level on this page
		int idx = CurrentIndex();
		if (idx >= totalLevels)
		{
			// wrap back to last valid
			_currentPage = 0;
			_gridRow = 0;
			_gridCol = 0;
		}
		LOG("JellyMenuBetter::HandleEvents right col=%d row=%d page=%d idx=%d\n", _gridCol, _gridRow, _currentPage, CurrentIndex());
		_levelImage->SetTexture(_levelManager->GetThumb(_sceneFiles[CurrentIndex()]));
		_audioHelper->PlayHitSound();
	}

	// Left: move col back, wrap to prev page
	if (_inputHelper->MenuActionPressed(MenuAction::MenuLeft))
	{
		_gridCol--;
		if (_gridCol < 0)
		{
			_gridCol = GRID_COLS - 1;
			_currentPage--;
			if (_currentPage < 0)
				_currentPage = _totalPages - 1;
			LOG("JellyMenuBetter::HandleEvents page back -> page=%d\n", _currentPage);
		}
		// clamp if on last page and cell is empty
		int idx = CurrentIndex();
		if (idx >= totalLevels)
		{
			// snap to last valid level
			int lastIdx = totalLevels - 1;
			_currentPage = lastIdx / PAGE_SIZE;
			_gridRow      = (lastIdx % PAGE_SIZE) / GRID_COLS;
			_gridCol      = lastIdx % GRID_COLS;
		}
		LOG("JellyMenuBetter::HandleEvents left col=%d row=%d page=%d idx=%d\n", _gridCol, _gridRow, _currentPage, CurrentIndex());
		_levelImage->SetTexture(_levelManager->GetThumb(_sceneFiles[CurrentIndex()]));
		_audioHelper->PlayHitSound();
	}

	// Down: move row forward
	if (_inputHelper->MenuActionPressed(MenuAction::MenuDown))
	{
		_gridRow++;
		if (_gridRow >= GRID_ROWS)
			_gridRow = 0;
		// clamp
		int idx = CurrentIndex();
		if (idx >= totalLevels)
		{
			_gridRow = 0;
			_gridCol = 0;
		}
		LOG("JellyMenuBetter::HandleEvents down col=%d row=%d page=%d idx=%d\n", _gridCol, _gridRow, _currentPage, CurrentIndex());
		_levelImage->SetTexture(_levelManager->GetThumb(_sceneFiles[CurrentIndex()]));
		_audioHelper->PlayHitSound();
	}

	// Up: move row back
	if (_inputHelper->MenuActionPressed(MenuAction::MenuUp))
	{
		_gridRow--;
		if (_gridRow < 0)
			_gridRow = GRID_ROWS - 1;
		// clamp
		int idx = CurrentIndex();
		if (idx >= totalLevels)
		{
			int lastIdx = totalLevels - 1;
			_gridRow = (lastIdx % PAGE_SIZE) / GRID_COLS;
			_gridCol = lastIdx % GRID_COLS;
		}
		LOG("JellyMenuBetter::HandleEvents up col=%d row=%d page=%d idx=%d\n", _gridCol, _gridRow, _currentPage, CurrentIndex());
		_levelImage->SetTexture(_levelManager->GetThumb(_sceneFiles[CurrentIndex()]));
		_audioHelper->PlayHitSound();
	}

	// Accept: launch level
	if (_inputHelper->MenuActionPressed(MenuAction::MenuAccept))
	{
		int idx = CurrentIndex();
		LOG("JellyMenuBetter::HandleEvents accept level idx=%d name=%s\n", idx, _sceneFiles[idx].c_str());

		_inputHelper->Update();

		_jellyGame = new JellyGame(_core);
		_jellyGame->Init();
		_jellyGame->LoadLevel(_levelManager, _sceneFiles[idx], _levelManager->GetLevelFile(_sceneFiles[idx]), "Assets/Jelly/car_and_truck.car");
		_jellyGame->SetChassisTextures(_levelManager->GetCarImage(_carSkins[carcurrentPosition].chassisSmall), _levelManager->GetCarImage(_carSkins[carcurrentPosition].chassisBig));
		_jellyGame->SetTireTextures(_levelManager->GetCarImage(_carSkins[carcurrentPosition].tireSmall), _levelManager->GetCarImage(_carSkins[carcurrentPosition].tireBig));

		manager->PushState(_jellyGame);
		_audioHelper->PlayMusic();
		return;
	}

	// Car skin cycling (Tire action = prev, Back = next - kept from original)
	if (_inputHelper->ActionPressed(CarAction::Tire))
	{
		carcurrentPosition--;
		if (carcurrentPosition < 0)
			carcurrentPosition = (int)_carSkins.size() - 1;
		LOG("JellyMenuBetter::HandleEvents skin prev=%d\n", carcurrentPosition);
		_car->SetChassisTextures(_levelManager->GetCarImage(_carSkins[carcurrentPosition].chassisSmall), _levelManager->GetCarImage(_carSkins[carcurrentPosition].chassisBig));
		_car->SetTireTextures(_levelManager->GetCarImage(_carSkins[carcurrentPosition].tireSmall), _levelManager->GetCarImage(_carSkins[carcurrentPosition].tireBig));
	}

	if (_inputHelper->MenuActionPressed(MenuAction::MenuBack))
	{
		carcurrentPosition++;
		if (carcurrentPosition >= (int)_carSkins.size())
			carcurrentPosition = 0;
		LOG("JellyMenuBetter::HandleEvents skin next=%d\n", carcurrentPosition);
		_car->SetChassisTextures(_levelManager->GetCarImage(_carSkins[carcurrentPosition].chassisSmall), _levelManager->GetCarImage(_carSkins[carcurrentPosition].chassisBig));
		_car->SetTireTextures(_levelManager->GetCarImage(_carSkins[carcurrentPosition].tireSmall), _levelManager->GetCarImage(_carSkins[carcurrentPosition].tireBig));
	}

	// Car driving in menu
	if (_inputHelper->ActionPressed(CarAction::Map))
		_car->Transform();

	if (_inputHelper->ActionHold(CarAction::Left))
		_car->setTorque(-1);

	if (_inputHelper->ActionHold(CarAction::Right))
		_car->setTorque(1);

	_car->mChassis->torque = 0.0f;

	if (_inputHelper->ActionHold(CarAction::RotateLeft))
		_car->mChassis->torque = -1.0f;

	if (_inputHelper->ActionHold(CarAction::RotateRight))
		_car->mChassis->torque = 1.0f;

	_inputHelper->Update();
}

void JellyMenuBetter::Update(GameManager* manager)
{
	_dt = _timer->GetDelta();
	_focusPhase += _dt * 4.0f;
	_focusPulse = 0.5f + 0.5f * sinf(_focusPhase);

	if (!_inputHelper->ActionHold(CarAction::Left) && !_inputHelper->ActionHold(CarAction::Right))
		_car->setTorque(0);

	for (int i = 0; i < 6; i++)
	{
		_world->update(0.004f);

		for (size_t j = 0; j < _gameBodies.size(); j++)
			_gameBodies[j]->Update(0.004f);

		_car->clearForces();
		_car->update(0.004f);
	}

	if (_car->getPosition().Y < _levelManager->GetLevelLine())
	{
		Vector2 pos   = _levelManager->GetCarStartPos();
		Vector2 scale = Vector2(1.0f, 1.0f);
		_car->getChassisBody()->setPositionAngle(pos, 0.0f, scale);
		_car->getTire(0)->setPositionAngle(pos, 0.0f, scale);
		_car->getTire(1)->setPositionAngle(pos, 0.0f, scale);
	}
}

void JellyMenuBetter::Draw(GameManager* manager)
{
	RectangleDrawer* ui = RectangleDrawer::Instance();

	_renderManager->StartFrame();
	_renderManager->ClearScreen();

	int screenW = _renderManager->GetWidth();
	int screenH = _renderManager->GetHeight();

	ui->DrawPaperBackground(_backSprite, screenW, screenH, _projection);

	// --- physics car preview (bottom strip) ---
	for (size_t i = 0; i < _gameBodies.size(); i++)
		_gameBodies[i]->Draw(_jellyProjection);
	_car->Draw(_jellyProjection);

	// --- layout constants ---
	const int CELL_W = 88;
	const int CELL_H = 72;
	const int GRID_WIDTH = GRID_COLS * CELL_W;
	const int GRID_HEIGHT = GRID_ROWS * CELL_H;

	float thumbScale = 0.95f;
	int thumbW = (int)(_levelImage->GetTexture()->GetWidth() * thumbScale);
	int thumbH = (int)(_levelImage->GetTexture()->GetHeight() * thumbScale);

	int spacing = 36;
	int totalWidth = GRID_WIDTH + spacing + thumbW + 48;
	int contentStartX = (screenW - totalWidth) / 2;
	int gridStartX = contentStartX + 24;

	// --- title bar ---
	int titlePanelW = 520;
	int titlePanelH = 72;
	int titlePanelX = (screenW - titlePanelW) / 2;
	int titlePanelY = 28;
	ui->DrawPanel(titlePanelX, titlePanelY, titlePanelW, titlePanelH, UiTheme::BarRadius(), _projection);

	_titleFont->AddText("JELLY CAR",
		UiLayout::CenterX(titlePanelX, titlePanelW),
		UiLayout::BaselineInPanel(titlePanelY, titlePanelH, UiLayout::TitleFontSize),
		UiTheme::TextHighlight(), FontCenter);

	// --- selected level name ---
	int selIdx = CurrentIndex();
	int levelPanelW = 480;
	int levelPanelH = 40;
	int levelPanelX = (screenW - levelPanelW) / 2;
	int levelPanelY = titlePanelY + titlePanelH + 8;
	ui->DrawPanel(levelPanelX, levelPanelY, levelPanelW, levelPanelH, UiTheme::BarRadius(), _projection);

	_menuFont->AddText(_sceneFiles[selIdx],
		UiLayout::CenterX(levelPanelX, levelPanelW),
		UiLayout::BaselineCenter(levelPanelY, levelPanelH, UiLayout::MenuFontSize),
		UiTheme::TextAccent(), FontCenter);

	// --- main grid panel ---
	const int pageFooterH = 34;
	int panelW = totalWidth;
	int panelH = GRID_HEIGHT + pageFooterH;
	int panelX = contentStartX;
	int panelY = levelPanelY + levelPanelH + 10;
	int gridStartY = panelY + 14;
	ui->DrawPanel(panelX, panelY, panelW, panelH, UiTheme::PanelRadius(), _projection);

	int totalLevels = (int)_sceneFiles.size();
	int pageBase = _currentPage * PAGE_SIZE;

	// --- grid cells ---
	for (int row = 0; row < GRID_ROWS; row++)
	{
		for (int col = 0; col < GRID_COLS; col++)
		{
			int idx = pageBase + row * GRID_COLS + col;
			if (idx >= totalLevels)
				continue;

			bool selected = (col == _gridCol && row == _gridRow);
			int boxW = CELL_W - 8;
			int boxH = CELL_H - 8;
			int boxX = gridStartX + col * CELL_W;
			int boxY = gridStartY + row * CELL_H;

			ui->DrawCell(boxX, boxY, boxW, boxH, selected, _focusPulse, _projection);

			int cx = UiLayout::CenterX(boxX, boxW);

			char numBuf[8];
			sprintf(numBuf, "%d", idx + 1);
			glm::vec3 nameCol = selected ? UiTheme::TextHighlight() : UiTheme::TextPrimary();

			char timeBuf[12];
			float t = _levelManager->GetTime(_sceneFiles[idx]);
			if (t < 999.0f)
				sprintf(timeBuf, "%.1fs", t);
			else
				sprintf(timeBuf, "--");

			glm::vec3 timeCol = selected ? UiTheme::TextSuccess() : UiTheme::TextMuted();
			int timeY = UiLayout::BaselineRow(boxY, boxH, 1, 2, UiLayout::SmallFontSize);
			_smallFont->AddText(timeBuf, cx, timeY, timeCol, FontCenter);

			int numY = UiLayout::BaselineBetweenTopAndLine(boxY, timeY, UiLayout::MenuFontSize);
			_menuFont->AddText(numBuf, cx, numY, nameCol, FontCenter);
		}
	}

	// page indicator — centered in footer band below grid
	{
		char pageBuf[32];
		sprintf(pageBuf, "Page %d / %d", _currentPage + 1, _totalPages);
		int gridBottom = gridStartY + GRID_HEIGHT;
		int footerY = gridBottom;
		int footerH = (panelY + panelH) - gridBottom;
		_smallFont->AddText(pageBuf,
			UiLayout::CenterX(panelX, panelW),
			UiLayout::BaselineCenter(footerY, footerH, UiLayout::SmallFontSize),
			UiTheme::TextMuted(), FontCenter);
	}

	// --- right panel: thumbnail + stats ---
	{
		int panelRight = panelX + panelW;
		int imageStartY = gridStartY;
		int detailPanelX = gridStartX + GRID_WIDTH + spacing - 16;
		int detailPanelW = panelRight - detailPanelX;
		int detailPanelH = thumbH + 100;
		int detailPanelY = imageStartY - 12;
		ui->DrawPanel(detailPanelX, detailPanelY, detailPanelW, detailPanelH, UiTheme::PanelRadius(), _projection);

		_levelImage->SetScale(glm::vec2(thumbScale, thumbScale));
		int thumbCX = UiLayout::CenterX(detailPanelX, detailPanelW);
		int thumbCY = imageStartY + thumbH / 2;
		_levelImage->SetPosition(glm::vec2(thumbCX, thumbCY));
		_levelImage->Draw(_projection);
		_levelImage->SetScale(glm::vec2(1.0f, 1.0f));

		int statsTop = imageStartY + thumbH + 12;
		int statsH = (detailPanelY + detailPanelH) - statsTop - 8;
		int leftCx = detailPanelX + detailPanelW / 4;
		int rightCx = detailPanelX + 3 * detailPanelW / 4;

		char bufTime[16], bufJump[16];
		float recTime = _levelManager->GetTime(_sceneFiles[selIdx]);
		float recJump = _levelManager->GetJump(_sceneFiles[selIdx]);
		if (recTime >= 999.0f)
			sprintf(bufTime, "--.--s");
		else
			sprintf(bufTime, "%.2fs", recTime);
		if (recJump <= 0.0f)
			sprintf(bufJump, "--.--m");
		else
			sprintf(bufJump, "%.2fm", recJump);

		int statRowY = UiLayout::BaselineRow(statsTop, statsH, 0, 2, UiLayout::MenuFontSize);
		_menuFont->AddText(bufTime, leftCx, statRowY, UiTheme::TextSuccess(), FontCenter);
		_menuFont->AddText(bufJump, rightCx, statRowY, UiTheme::TextHighlight(), FontCenter);

		_smallFont->AddText(_carSkins[carcurrentPosition].name,
			UiLayout::CenterX(detailPanelX, detailPanelW),
			UiLayout::BaselineRow(statsTop, statsH, 1, 2, UiLayout::SmallFontSize),
			UiTheme::TextAccent(), FontCenter);
	}

	// --- bottom bar ---
	ui->DrawBottomBar(screenW, screenH, _projection);

	{
		int barX, barY, barW, barH;
		ui->GetBottomBarRect(screenW, screenH, barX, barY, barW, barH);
		int halfW = barW / 2;

		UiPrompt::DrawPromptInSlot(ui, _smallFont, _menuFont, _projection,
			barX, halfW, barY, barH,
			_inputHelper->MenuActionLabel(MenuAction::MenuPause).c_str(), "Options");

		UiPrompt::DrawPromptInSlot(ui, _smallFont, _menuFont, _projection,
			barX + halfW, halfW, barY, barH,
			_inputHelper->MenuActionLabel(MenuAction::MenuExit).c_str(), "Exit");
	}

	_menuFont->Draw(_projection);
	_titleFont->Draw(_projection);
	_smallFont->Draw(_projection);

	_renderManager->EndFrame();
}
