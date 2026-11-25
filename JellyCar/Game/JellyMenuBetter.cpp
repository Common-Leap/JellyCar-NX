#include "JellyMenuBetter.h"

#include "JellySplash.h"
#include "JellyGame.h"
#include "JellyOptions.h"

#include "../Utils/JellyHelper.h"

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

	//grid cell box, drawn by the old menu
	_boxSprite = new Sprite("roundedbox", "Assets/Jelly/Texture/rounded_box.png", "Assets/Shaders/sprite", "Assets/Shaders/sprite");

	_jellyGame = nullptr;
	_jellyOptions = nullptr;
	_launchLevel = false;
	_confirmExit = false;
	_confirmYes = true;
	_cursorX = 0.0f;
	_cursorY = 0.0f;
	_cursorPlaced = false;
	_focusPulse = 0.0f;
	_focusPhase = 0.0f;

	//load menu level physics
	_world = new World();
	_levelManager->InitPhysic(_world);
	_levelManager->LoadCompiledLevel(_world, "menu.scene", "Assets/Jelly/car_and_truck.car");

	_car = _levelManager->GetCar();
	carcurrentPosition = 0;
	UpdateCarSkin();

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
	delete _boxSprite;

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
		GameResult result = _jellyGame->GetResult();

		if (result == ResultNextLevel)
		{
			SelectLevel(CurrentIndex() + 1);
		}

		_launchLevel = result != ResultLevelSelect;

		delete _jellyGame;
		_jellyGame = nullptr;
	}

	delete _jellyOptions;
	_jellyOptions = nullptr;

	//the time away does not count
	_timer->GetDelta();

	_inputHelper->Update();
}

void JellyMenuBetter::GamePause()
{
}

void JellyMenuBetter::GameResume()
{
}

void JellyMenuBetter::SelectLevel(int index)
{
	int last = (int)_sceneFiles.size() - 1;

	if (index < 0)
		index = 0;

	if (index > last)
		index = last;

	_currentPage = index / PAGE_SIZE;
	_gridRow = (index % PAGE_SIZE) / GRID_COLS;
	_gridCol = index % GRID_COLS;

	_levelImage->SetTexture(_levelManager->GetThumb(_sceneFiles[index]));
}

int JellyMenuBetter::LastRowOnPage()
{
	int onPage = (int)_sceneFiles.size() - _currentPage * PAGE_SIZE;
	if (onPage > PAGE_SIZE)
		onPage = PAGE_SIZE;

	return (onPage - 1) / GRID_COLS;
}

void JellyMenuBetter::MoveHorizontal(int step)
{
	int total = (int)_sceneFiles.size();
	int col = _gridCol + step;

	if (col >= 0 && col < GRID_COLS && CurrentIndex() + step < total)
	{
		SelectLevel(CurrentIndex() + step);
	}
	else
	{
		//same row on the page next to this one
		_currentPage = (_currentPage + step + _totalPages) % _totalPages;
		_gridCol = step > 0 ? 0 : GRID_COLS - 1;

		if (_gridRow > LastRowOnPage())
			_gridRow = LastRowOnPage();

		SelectLevel(CurrentIndex());
	}

	_audioHelper->PlayHitSound();
}

void JellyMenuBetter::MoveVertical(int step)
{
	int rows = LastRowOnPage() + 1;
	_gridRow = (_gridRow + step + rows) % rows;

	SelectLevel(CurrentIndex());
	_audioHelper->PlayHitSound();
}

void JellyMenuBetter::ChangePage(int step)
{
	if (_totalPages < 2)
	{
		return;
	}

	_currentPage = (_currentPage + step + _totalPages) % _totalPages;

	SelectLevel(CurrentIndex());
	_audioHelper->PlayHitSound();
}

void JellyMenuBetter::UpdateCarSkin()
{
	SkinInfo& skin = _carSkins[carcurrentPosition];

	_car->SetChassisTextures(_levelManager->GetCarImage(skin.chassisSmall), _levelManager->GetCarImage(skin.chassisBig));
	_car->SetTireTextures(_levelManager->GetCarImage(skin.tireSmall), _levelManager->GetCarImage(skin.tireBig));
}

void JellyMenuBetter::ChangeSkin(int step)
{
	int count = (int)_carSkins.size();
	carcurrentPosition = (carcurrentPosition + step + count) % count;

	UpdateCarSkin();
}

void JellyMenuBetter::LaunchLevel(GameManager* manager)
{
	int idx = CurrentIndex();
	LOG("JellyMenuBetter::LaunchLevel idx=%d name=%s\n", idx, _sceneFiles[idx].c_str());

	_inputHelper->Update();

	SkinInfo& skin = _carSkins[carcurrentPosition];

	_jellyGame = new JellyGame(_core);
	_jellyGame->Init();
	_jellyGame->LoadLevel(_levelManager, _sceneFiles[idx], _levelManager->GetLevelFile(_sceneFiles[idx]), "Assets/Jelly/car_and_truck.car");
	_jellyGame->SetChassisTextures(_levelManager->GetCarImage(skin.chassisSmall), _levelManager->GetCarImage(skin.chassisBig));
	_jellyGame->SetTireTextures(_levelManager->GetCarImage(skin.tireSmall), _levelManager->GetCarImage(skin.tireBig));
	_jellyGame->SetNextLevelAvailable(idx + 1 < (int)_sceneFiles.size());

	manager->PushState(_jellyGame);
}

void JellyMenuBetter::OpenOptions(GameManager* manager)
{
	_jellyOptions = new JellyOptions(_core);
	manager->PushState(_jellyOptions);
}

void JellyMenuBetter::HandleConfirmExit(GameManager* manager, int touched)
{
	if (_inputHelper->MenuActionPressed(MenuAction::MenuLeft))
		_confirmYes = true;

	if (_inputHelper->MenuActionPressed(MenuAction::MenuRight))
		_confirmYes = false;

	bool accept = _inputHelper->MenuActionPressed(MenuAction::MenuAccept);

	if (touched == TouchExitYes || (accept && _confirmYes))
	{
		LOG("JellyMenuBetter::HandleConfirmExit exit\n");
		manager->Quit();
	}
	else if (touched == TouchExitNo || accept
		|| _inputHelper->MenuActionPressed(MenuAction::MenuBack)
		|| _inputHelper->MenuActionPressed(MenuAction::MenuExit))
	{
		_confirmExit = false;
	}
}

void JellyMenuBetter::HandleEvents(GameManager* manager)
{
	glm::vec2 point;
	int touched = TouchTargets::None;
	if (_inputHelper->TouchPressed(point))
	{
		touched = _touch.Find(point);
	}

	//the left stick drives the car, the d-pad selects the level
	_car->setTorque(_confirmExit ? 0.0f : _inputHelper->StickDriveAxis());
	_car->mChassis->torque = _confirmExit ? 0.0f : _inputHelper->StickRotateAxis();

	if (_confirmExit)
	{
		HandleConfirmExit(manager, touched);
		_inputHelper->Update();
		return;
	}

	if (touched == TouchExit || _inputHelper->MenuActionPressed(MenuAction::MenuExit))
	{
		_confirmExit = true;
		_confirmYes = true;
		_inputHelper->Update();
		return;
	}

	if (touched == TouchOptions || _inputHelper->MenuActionPressed(MenuAction::MenuPause))
	{
		OpenOptions(manager);
		return;
	}

	//level
	if (_inputHelper->MenuActionPressed(MenuAction::GridRight))
		MoveHorizontal(1);

	if (_inputHelper->MenuActionPressed(MenuAction::GridLeft))
		MoveHorizontal(-1);

	if (_inputHelper->MenuActionPressed(MenuAction::GridDown))
		MoveVertical(1);

	if (_inputHelper->MenuActionPressed(MenuAction::GridUp))
		MoveVertical(-1);

	if (touched == TouchPageNext || _inputHelper->MenuActionPressed(MenuAction::PageNext))
		ChangePage(1);

	if (touched == TouchPagePrevious || _inputHelper->MenuActionPressed(MenuAction::PagePrevious))
		ChangePage(-1);

	//car skin
	if (touched == TouchSkinNext || _inputHelper->MenuActionPressed(MenuAction::SkinNext))
		ChangeSkin(1);

	if (touched == TouchSkinPrevious || _inputHelper->MenuActionPressed(MenuAction::SkinPrevious))
		ChangeSkin(-1);

	//touch selects a level, and starts it when it is selected already
	bool play = touched == TouchPlay || _inputHelper->MenuActionPressed(MenuAction::MenuAccept);

	if (touched >= TouchLevel)
	{
		play = touched - TouchLevel == CurrentIndex();

		if (!play)
		{
			SelectLevel(touched - TouchLevel);
			_audioHelper->PlayHitSound();
		}
	}

	if (play)
	{
		LaunchLevel(manager);
		_audioHelper->PlayMusic();
		return;
	}

	if (_inputHelper->ActionPressed(CarAction::Transform))
		_car->Transform();

	_inputHelper->Update();
}

void JellyMenuBetter::Update(GameManager* manager)
{
	//retry or next level: start it here, so that the level select is not drawn in between
	if (_launchLevel)
	{
		_launchLevel = false;
		LaunchLevel(manager);
		return;
	}

	_dt = _timer->GetDelta();
	_focusPhase += _dt * 4.0f;
	_focusPulse = 0.5f + 0.5f * sinf(_focusPhase);

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
