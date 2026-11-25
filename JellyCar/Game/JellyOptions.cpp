#include "JellyOptions.h"

#include <Andromeda/Utils/Logger.h>
#include "../Utils/JellyHelper.h"

#include <stdio.h>
#include <sstream>

JellyOptions::JellyOptions(JellyCore* core)
{
	_core = core;
}

void JellyOptions::Init()
{
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
	//_audioHelper->PlayMusic();

	//level manager
	_levelManager = new LevelManager();
	_levelManager->SetAssetsLocation("Assets/Jelly/");


	//background paper sprite
	_backSprite = new Sprite("paper", "Assets/Jelly/Texture/paper.png", "Assets/Shaders/sprite", "Assets/Shaders/sprite");
	_backSelectSprite = new Sprite("selectBack", "Assets/Jelly/Texture/back.png", "Assets/Shaders/sprite", "Assets/Shaders/sprite");

	_backRoundSprite = new Sprite("selectRoundBack", "Assets/Jelly/Texture/roundBack.png", "Assets/Shaders/sprite", "Assets/Shaders/sprite");
	_leftSprite = new Sprite("selectLeft", "Assets/Jelly/Texture/left.png", "Assets/Shaders/sprite", "Assets/Shaders/sprite");
	_rightSprite = new Sprite("selectRight", "Assets/Jelly/Texture/right.png", "Assets/Shaders/sprite", "Assets/Shaders/sprite");

	_barSprite = new Sprite("optionsBar", "Assets/Jelly/Texture/bar.png", "Assets/Shaders/sprite", "Assets/Shaders/sprite");
	_iconSprite = new Sprite("optionsIcon", "Assets/Jelly/Texture/gamepad.png", "Assets/Shaders/sprite", "Assets/Shaders/sprite");

	_barBlueSprite = new Sprite("optionsBarBlue", "Assets/Jelly/Texture/barBlue.png", "Assets/Shaders/sprite", "Assets/Shaders/sprite");

	//


	_creditsTexture = TextureManager::Instance()->LoadFromFile("Assets/Jelly/Texture/credits.png");
	_libsTexture = TextureManager::Instance()->LoadFromFile("Assets/Jelly/Texture/libs.png");
	_gamepadTexture = TextureManager::Instance()->LoadFromFile("Assets/Jelly/Texture/gamepad.png");
	_secretTexture = TextureManager::Instance()->LoadFromFile("Assets/Jelly/Texture/secret.png");
	_volumeTexture = TextureManager::Instance()->LoadFromFile("Assets/Jelly/Texture/volume.png");

	//load menu level
	_world = new World();

	_levelManager->InitPhysic(_world);
	//_levelManager->LoadLevel(_world, "options_scene.scene", "");
	_levelManager->LoadCompiledLevel(_world, "options_scene.scene", "");

	//level elements
	_gameBodies = _levelManager->GetLevelBodies();

	_screenBounds = glm::vec4(-20.0f + 0, 0 + 20.0f, -4.2f - 5, -5 + 18.2f);

	_jellyProjection = glm::ortho(-20.0f + 0, 0 + 20.0f, -4.2f - 5, -5 + 18.2f, -1.0f, 1.0f);

	//timer
	_dt = 0;
	_timer = new Timer();

	InitCredits();
	InitLibs();
	InitActionNames();

	AddMenuItem(JellyOptionsState::Controls, "Controls", "options_keys", _gamepadTexture);
	AddMenuItem(JellyOptionsState::Sound, "Sound", "options_sound", _volumeTexture);
	AddMenuItem(JellyOptionsState::Credits, "Credits", "options_credits", _creditsTexture);
	AddMenuItem(JellyOptionsState::Libs, "Libraries", "options_libs", _libsTexture);

	for (size_t i = 0; i < _gameBodies.size(); i++)
	{
		if (_gameBodies[i]->GetName() == "options_secret")
		{
			_gameBodies[i]->SetTexture(_secretTexture);
		}
	}

	_menuSelected = 0;
	_selctedPosition = 0;
	_soundPosition = 0;

	ArrangeMenu();
	ResetScroll();

	_alphaScale = 1.0f;

	//binding
	_changeBinding = false;

	_optionsCarLevel = AudioHelper::Instance()->GetVolume(AudioHelper::AudioHelperSoundEnum::Car) * 10.0f;
	_optionsSoundLevel = AudioHelper::Instance()->GetVolume(AudioHelper::AudioHelperSoundEnum::Sounds) * 10.0f;
	_optionsMusicLevel = AudioHelper::Instance()->GetVolume(AudioHelper::AudioHelperSoundEnum::Music) * 10.0f;

	_optionsState = JellyOptionsState::Menu;

	dragBody = NULL;	
}

void JellyOptions::InitCredits()
{
	_credits.push_back(Text("Thanks", 0));
	_credits.push_back(Text("Walaber for original game", 82));
	_credits.push_back(Text("Shadow for car skins", 142));
	_credits.push_back(Text("Ruben Wolfe, SMOKE, TheFloW", 202));
	_credits.push_back(Text("for support and testing", 242));
	_credits.push_back(Text("Rinnegatamante and EasyRPG Team", 302));
	_credits.push_back(Text("for the Audio Decoder used for Vita sound module", 342));
	_credits.push_back(Text("St3f  for splash screen", 402));
	_credits.push_back(Text("Team Molecule for HENkaku", 462));
	_credits.push_back(Text("Everybody who contributed to vitasdk", 522));
	_credits.push_back(Text("Switchbrew team", 582));
	_credits.push_back(Text("Everybody who contributed to libnx", 642));
}

void JellyOptions::InitLibs()
{
	_libs.push_back(Text("Libs used for PC/Vita/Switch", 0));
	_libs.push_back(Text("freetype", 82));
	_libs.push_back(Text("freetype_gl", 142));
	_libs.push_back(Text("glad", 202));
	_libs.push_back(Text("glew", 262));
	_libs.push_back(Text("glfw", 322));
	_libs.push_back(Text("glm", 382));
	_libs.push_back(Text("irrKlang", 442));
	_libs.push_back(Text("JellyPhysics", 502));
	_libs.push_back(Text("ogg", 562));
	_libs.push_back(Text("SDL2", 622));
	_libs.push_back(Text("stb", 682));
	_libs.push_back(Text("speexdsp", 742));
	_libs.push_back(Text("tinyxml", 802));
	_libs.push_back(Text("vorbisfile", 862));
	_libs.push_back(Text("vorbisenc", 922));
	_libs.push_back(Text("vorbis", 982));
}

void JellyOptions::InitActionNames()
{
	_carActions.push_back(CarAction::Left);
	_carActions.push_back(CarAction::Right);
	_carActions.push_back(CarAction::RotateLeft);
	_carActions.push_back(CarAction::RotateRight);

	_carActions.push_back(CarAction::Transform);
	_carActions.push_back(CarAction::Ballon);
	_carActions.push_back(CarAction::Tire);
	_carActions.push_back(CarAction::Map);

	_actionTranslation.insert(std::pair<CarAction, std::string>(CarAction::Left, "Left"));
	_actionTranslation.insert(std::pair<CarAction, std::string>(CarAction::Right, "Right"));
	_actionTranslation.insert(std::pair<CarAction, std::string>(CarAction::RotateLeft, "Rotate Left"));
	_actionTranslation.insert(std::pair<CarAction, std::string>(CarAction::RotateRight, "Rotate Right"));

	_actionTranslation.insert(std::pair<CarAction, std::string>(CarAction::Transform, "Transform"));
	_actionTranslation.insert(std::pair<CarAction, std::string>(CarAction::Ballon, "Balloon"));
	_actionTranslation.insert(std::pair<CarAction, std::string>(CarAction::Tire, "Sticky tire"));
	_actionTranslation.insert(std::pair<CarAction, std::string>(CarAction::Map, "Map"));
}

void JellyOptions::Enter()
{

}

void JellyOptions::CleanUp()
{
	if (_levelManager != 0)
	{
		//clear level
		_levelManager->ClearLevel(_world);

		//remove level manager
		delete _levelManager;
	}

	//remove physic world
	delete _world;
	_gameBodies.clear();

    delete _timer;

	delete _backSprite;
	delete _backSelectSprite;
	delete _backRoundSprite;
	delete _leftSprite;
	delete _rightSprite;
	delete _barSprite;
	delete _barBlueSprite;
	delete _iconSprite;

    _credits.clear();
    _libs.clear();
	_actionTranslation.clear();
}

void JellyOptions::Pause()
{

}

void JellyOptions::Resume()
{

}

void JellyOptions::GamePause()
{

}

void JellyOptions::GameResume()
{

}

void JellyOptions::AddMenuItem(JellyOptionsState state, std::string label, std::string bodyName, Texture* image)
{
	MenuItem item;
	item.State = state;
	item.Label = label;
	item.BodyName = bodyName;
	item.Image = image;
	item.Body = NULL;

	for (size_t i = 0; i < _gameBodies.size(); i++)
	{
		if (_gameBodies[i]->GetName() == bodyName)
		{
			_gameBodies[i]->SetTexture(image);
			item.Body = _gameBodies[i];
		}
	}

	_menuItems.push_back(item);
}

JellyOptions::JellyOptionsState JellyOptions::CurrentPage()
{
	if (_optionsState == Menu && !_showBodies)
	{
		return _menuItems[_menuSelected].State;
	}

	return _optionsState;
}

std::vector<Text>& JellyOptions::ScrollText()
{
	return CurrentPage() == Libs ? _libs : _credits;
}

void JellyOptions::ResetScroll()
{
	//come in from below, or start at the top when the page is already visible
	_scrollPosition = _showBodies ? _scrollBottom : _scrollTop + 40.0f;
}

void JellyOptions::SelectItem(int item)
{
	if (item < 0 || item >= (int)_menuItems.size() || item == _menuSelected)
	{
		return;
	}

	_menuSelected = item;
	ResetScroll();
}

void JellyOptions::OpenPage(JellyOptionsState state)
{
	for (size_t i = 0; i < _menuItems.size(); i++)
	{
		if (_menuItems[i].State == state)
		{
			SelectItem(i);
		}
	}

	_optionsState = state;
	_alphaScale = 1.0f;

	if (state == Controls)
	{
		_selctedPosition = 0;
	}
	else if (state == Sound)
	{
		_soundPosition = 0;
		_audioHelper->PlayFastEngine();
	}
	else
	{
		ResetScroll();
	}
}

bool JellyOptions::HandleBodyTouch()
{
	if (_inputHelper->GetTouchCount() == 0)
	{
		dragBody = NULL;
		dragPoint = -1;
		return false;
	}

	glm::vec2 touch = _inputHelper->TouchToScreen(_screenBounds, _inputHelper->GetTouchPosition(0));

	dragX = touch.x;
	dragY = touch.y;

	if (dragBody != NULL)
	{
		PointMass *pm = dragBody->getPointMass(dragPoint);
		dragBody->setDragForce(VectorTools::calculateSpringForce(pm->Position, pm->Velocity, Vector2(dragX, dragY), Vector2(0, 0), 0.0f, 100.0f, 10.0f), dragPoint);
		return false;
	}

	int body;
	_world->getClosestPointMass(Vector2(dragX, dragY), body, dragPoint);
	dragBody = _world->getBody(body);

	//click testing
	for (size_t i = 0; i < _menuItems.size(); i++)
	{
		if (_menuItems[i].Body != NULL && _menuItems[i].Body->GetBody()->contains(Vector2(dragX, dragY)))
		{
			OpenPage(_menuItems[i].State);
			return true;
		}
	}

	return false;
}

void JellyOptions::HandleMenu(GameManager* manager, int touched)
{
	if (_showBodies && HandleBodyTouch())
	{
		return;
	}

	if (touched >= TouchItem && touched < TouchItem + (int)_menuItems.size())
	{
		OpenPage(_menuItems[touched - TouchItem].State);
		return;
	}

	//the bodies are side by side, the list goes down
	if (_inputHelper->MenuActionPressed(_showBodies ? MenuAction::MenuLeft : MenuAction::MenuUp))
	{
		SelectItem(_menuSelected - 1);
	}

	if (_inputHelper->MenuActionPressed(_showBodies ? MenuAction::MenuRight : MenuAction::MenuDown))
	{
		SelectItem(_menuSelected + 1);
	}

	if (_inputHelper->MenuActionPressed(MenuAction::MenuAccept))
	{
		OpenPage(_menuItems[_menuSelected].State);
		return;
	}

	if (touched == TouchBack || _inputHelper->MenuActionPressed(MenuAction::MenuBack))
	{
		manager->PopState();
	}
}

void JellyOptions::StartBinding(int position)
{
	_selctedPosition = position;
	_selectedAction = _carActions[position];
	_changeBinding = true;

	_inputHelper->UpdateAllInputs(true);
}

void JellyOptions::MoveBinding(int column, int row)
{
	const int rows = (int)_carActions.size() / 2;

	int x = _selctedPosition / rows + column;
	int y = _selctedPosition % rows + row;

	if (x >= 0 && x < 2 && y >= 0 && y < rows)
	{
		_selctedPosition = x * rows + y;
	}
}

void JellyOptions::HandleControls(int touched)
{
	if (_changeBinding)
	{
		if (_inputHelper->ChangeInputForAction(_selectedAction) != BindWaiting || touched != TouchTargets::None)
		{
			_inputHelper->UpdateAllInputs(false);
			_changeBinding = false;
		}

		return;
	}

	if (touched >= TouchBinding && touched < TouchBinding + (int)_carActions.size())
	{
		StartBinding(touched - TouchBinding);
		return;
	}

	if (_inputHelper->MenuActionPressed(MenuAction::MenuUp))
		MoveBinding(0, -1);

	if (_inputHelper->MenuActionPressed(MenuAction::MenuDown))
		MoveBinding(0, 1);

	if (_inputHelper->MenuActionPressed(MenuAction::MenuLeft))
		MoveBinding(-1, 0);

	if (_inputHelper->MenuActionPressed(MenuAction::MenuRight))
		MoveBinding(1, 0);

	if (touched == TouchReset || _inputHelper->MenuActionPressed(MenuAction::MenuReset))
	{
		_inputHelper->ResetCarControls();
	}

	if (_inputHelper->MenuActionPressed(MenuAction::MenuAccept))
	{
		StartBinding(_selctedPosition);
		return;
	}

	if (touched == TouchBack || _inputHelper->MenuActionPressed(MenuAction::MenuBack))
	{
		//save settings
		_inputHelper->SaveSettings();

		//go back to options menu
		_optionsState = Menu;
	}
}

int& JellyOptions::VolumeLevel(int position)
{
	if (position == 0)
		return _optionsCarLevel;

	if (position == 1)
		return _optionsSoundLevel;

	return _optionsMusicLevel;
}

void JellyOptions::SetVolumeLevel(int position, int level)
{
	int& current = VolumeLevel(position);

	if (level < 0 || level > 10 || level == current)
	{
		return;
	}

	current = level;

	if (position == 0)
	{
		_audioHelper->SetVolume(AudioHelper::AudioHelperSoundEnum::Car, (float)level / 10.0f);
	}
	else if (position == 1)
	{
		_audioHelper->SetVolume(AudioHelper::AudioHelperSoundEnum::Sounds, (float)level / 10.0f);
		_audioHelper->PlayHitSound();
	}
	else
	{
		_audioHelper->SetVolume(AudioHelper::AudioHelperSoundEnum::Music, (float)level / 10.0f);
	}
}

void JellyOptions::SelectVolume(int position)
{
	if (position < 0 || position > 2 || position == _soundPosition)
	{
		return;
	}

	_soundPosition = position;

	//the engine only runs while its volume is selected
	_audioHelper->StopEngineSound();

	if (_soundPosition == 0)
	{
		_audioHelper->PlayFastEngine();
	}
}

void JellyOptions::HandleSound(int touched)
{
	if (_inputHelper->MenuActionPressed(MenuAction::MenuUp))
		SelectVolume(_soundPosition - 1);

	if (_inputHelper->MenuActionPressed(MenuAction::MenuDown))
		SelectVolume(_soundPosition + 1);

	if (_inputHelper->MenuActionPressed(MenuAction::MenuLeft))
		SetVolumeLevel(_soundPosition, VolumeLevel(_soundPosition) - 1);

	if (_inputHelper->MenuActionPressed(MenuAction::MenuRight))
		SetVolumeLevel(_soundPosition, VolumeLevel(_soundPosition) + 1);

	//the bars can be dragged
	if (_volumeBarW > 0 && _inputHelper->GetTouchCount() > 0)
	{
		glm::vec2 touch = _inputHelper->GetTouchPosition(0);
		int id = _touch.Find(touch);

		if (id >= TouchVolume && id < TouchVolume + 3)
		{
			float level = (touch.x - _volumeBarX) * 10.0f / _volumeBarW + 0.5f;

			SelectVolume(id - TouchVolume);
			SetVolumeLevel(id - TouchVolume, level < 0.0f ? 0 : (level > 10.0f ? 10 : (int)level));
		}
	}

	if (touched == TouchBack || _inputHelper->MenuActionPressed(MenuAction::MenuBack))
	{
		//save settings
		_audioHelper->SaveSettings();

		//stop engine sound
		_audioHelper->StopEngineSound();

		//go back to menu options
		_optionsState = Menu;
	}
}

void JellyOptions::HandleText(int touched)
{
	if (_inputHelper->MenuActionPressed(MenuAction::MenuUp))
		_scrollPosition += 40.0f;

	if (_inputHelper->MenuActionPressed(MenuAction::MenuDown))
		_scrollPosition -= 40.0f;

	if (touched == TouchBack || _inputHelper->MenuActionPressed(MenuAction::MenuBack))
	{
		_optionsState = Menu;
	}
}

void JellyOptions::HandleEvents(GameManager* manager)
{
	glm::vec2 point;
	int touched = TouchTargets::None;
	if (_inputHelper->TouchPressed(point))
	{
		touched = _touch.Find(point);
	}

	switch (_optionsState)
	{
	case Menu:
		HandleMenu(manager, touched);
		break;
	case Controls:
		HandleControls(touched);
		break;
	case Sound:
		HandleSound(touched);
		break;
	default:
		HandleText(touched);
		break;
	}

	//going back deletes this object
	InputHelper::Instance()->Update();
}

void JellyOptions::Update(GameManager* manager)
{
	_dt = _timer->GetDelta();

	if (_optionsState == Menu && _showBodies)
	{
		//Update physic
		for (int i = 0; i < 6; i++)
		{
			_world->update(0.004f);

			for (size_t i = 0; i < _gameBodies.size(); i++)
				_gameBodies[i]->Update(0.004f);
		}
	}

	if (CurrentPage() == Credits || CurrentPage() == Libs)
	{
		_scrollPosition -= (_dt * 40);

		//start over once the last line is gone, and don't scroll back past the first one
		if (ScrollText().back().StartPosition + _scrollPosition < _scrollTop)
		{
			_scrollPosition = _scrollBottom;
		}

		if (_scrollPosition > _scrollBottom)
		{
			_scrollPosition = _scrollBottom;
		}
	}

	//update alpha scale
	_alphaScale += (_scaleFactor * _dt);

	if (_alphaScale > 1.0f)
	{
		_scaleFactor = -2.0f;
	}

	if (_alphaScale < 0.0f)
	{
		_scaleFactor = 2.0f;
	}
}
