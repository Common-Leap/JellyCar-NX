#include "InputHelper.h"
#include <Andromeda/Utils/Logger.h>
#include <Andromeda/FileSystem/FileManager.h>
#include "tinyxml.h"

#include <algorithm>
#include <chrono>
#include <cmath>

namespace AFS = Andromeda::FileSystem;

InputHelper* InputHelper::_inputHelper = 0;

namespace
{
	const double REPEAT_DELAY = 0.35;
	const double REPEAT_INTERVAL = 0.11;

	const float STICK_DEADZONE = 0.18f;
	const float STICK_FULL = 0.90f;

	double Now()
	{
		return std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count();
	}

	//remove two actions from a saved mapping when both use the given buttons
	void RemoveMapping(std::map<int, int>& mapping, CarAction first, Gamepad::Button firstButton, CarAction second, Gamepad::Button secondButton)
	{
		std::map<int, int>::iterator a = mapping.find(first);
		std::map<int, int>::iterator b = mapping.find(second);

		if (a != mapping.end() && b != mapping.end() && a->second == firstButton && b->second == secondButton)
		{
			mapping.erase(a);
			mapping.erase(second);
		}
	}

	float StickValue(float value)
	{
		float size = fabsf(value);
		if (size <= STICK_DEADZONE)
			return 0.0f;

		size = std::min(1.0f, (size - STICK_DEADZONE) / (STICK_FULL - STICK_DEADZONE));
		return value < 0.0f ? -size : size;
	}
}

void InputHelper::PrepareDefaultCarMapping()
{
	_carGamepadMapping.clear();

	_carGamepadMapping[CarAction::Left] = Gamepad::Button::LAnalogLeft;
	_carGamepadMapping[CarAction::Right] = Gamepad::Button::LAnalogRight;

	_carGamepadMapping[CarAction::RotateLeft] = Gamepad::Button::LTrigger;
	_carGamepadMapping[CarAction::RotateRight] = Gamepad::Button::RTrigger;

	_carGamepadMapping[CarAction::Transform] = Gamepad::Button::Cross;
	_carGamepadMapping[CarAction::Ballon] = Gamepad::Button::Circle;
	_carGamepadMapping[CarAction::Tire] = Gamepad::Button::Square;
	_carGamepadMapping[CarAction::Map] = Gamepad::Button::Triangle;
}

void InputHelper::PrepareGamepadMapping()
{
	//menu input
	_menuGamepadMapping[MenuAction::MenuAccept] = Gamepad::Button::Circle;
	_menuGamepadMapping[MenuAction::MenuBack] = Gamepad::Button::Cross;

	_menuGamepadMapping[MenuAction::MenuUp] = Gamepad::Button::LAnalogUp;
	_menuGamepadMapping[MenuAction::MenuDown] = Gamepad::Button::LAnalogDown;
	_menuGamepadMapping[MenuAction::MenuLeft] = Gamepad::Button::LAnalogLeft;
	_menuGamepadMapping[MenuAction::MenuRight] = Gamepad::Button::LAnalogRight;

	_menuGamepadMapping[MenuAction::GridUp] = Gamepad::Button::Up;
	_menuGamepadMapping[MenuAction::GridDown] = Gamepad::Button::Down;
	_menuGamepadMapping[MenuAction::GridLeft] = Gamepad::Button::Left;
	_menuGamepadMapping[MenuAction::GridRight] = Gamepad::Button::Right;

	_menuGamepadMapping[MenuAction::MenuPause] = Gamepad::Button::Select;
	_menuGamepadMapping[MenuAction::MenuExit] = Gamepad::Button::Start;

	_menuGamepadMapping[MenuAction::MenuRetry] = Gamepad::Button::Triangle;
	_menuGamepadMapping[MenuAction::MenuReset] = Gamepad::Button::Square;

	_menuGamepadMapping[MenuAction::PagePrevious] = Gamepad::Button::LTrigger;
	_menuGamepadMapping[MenuAction::PageNext] = Gamepad::Button::RTrigger;
	_menuGamepadMapping[MenuAction::SkinPrevious] = Gamepad::Button::ZLTrigger;
	_menuGamepadMapping[MenuAction::SkinNext] = Gamepad::Button::ZRTrigger;

	_menuAliases[MenuAction::MenuUp] = { Gamepad::Button::Up };
	_menuAliases[MenuAction::MenuDown] = { Gamepad::Button::Down };
	_menuAliases[MenuAction::MenuLeft] = { Gamepad::Button::Left };
	_menuAliases[MenuAction::MenuRight] = { Gamepad::Button::Right };

	//car input
	PrepareDefaultCarMapping();

	_carAliases[CarAction::Left] = { Gamepad::Button::LAnalogLeft, Gamepad::Button::Left };
	_carAliases[CarAction::Right] = { Gamepad::Button::LAnalogRight, Gamepad::Button::Right };
	_carAliases[CarAction::RotateLeft] = { Gamepad::Button::RAnalogLeft, Gamepad::Button::LTrigger, Gamepad::Button::ZLTrigger };
	_carAliases[CarAction::RotateRight] = { Gamepad::Button::RAnalogRight, Gamepad::Button::RTrigger, Gamepad::Button::ZRTrigger };

	//all gamepad buttons
	for (int key = 0; key < Gamepad::Button::Count; key++)
	{
		Gamepad::Button button = static_cast<Gamepad::Button>(key);
		_buttonPressed.insert(std::pair<Gamepad::Button, bool>(button, false));
	}
}

void InputHelper::PrepareSpriteGamepadMapping()
{
	_spriteShader = ShaderManager::Instance()->LoadFromFile("sprite", "Assets/Shaders/sprite", "Assets/Shaders/sprite", Textured);

	std::string imageFile = "Assets/Images/buttons_switch.png";

	//shapes
	_spriteGamepadMapping.insert(std::pair<Gamepad::Button, Sprite*>(Gamepad::Button::Triangle, new Sprite("_control1", TextureManager::Instance()->LoadFromFile(imageFile, TextureFilerType::LinearFilter), _spriteShader, glm::vec2(17.0f, 5.0f), glm::vec2(78.0f, 78.0f)) ));
	_spriteGamepadMapping.insert(std::pair<Gamepad::Button, Sprite*>(Gamepad::Button::Circle, new Sprite("_control2", TextureManager::Instance()->LoadFromFile(imageFile, TextureFilerType::LinearFilter), _spriteShader, glm::vec2(102.0f, 5.0f), glm::vec2(78.0f, 78.0f)) ));
	_spriteGamepadMapping.insert(std::pair<Gamepad::Button, Sprite*>(Gamepad::Button::Cross, new Sprite("_control3", TextureManager::Instance()->LoadFromFile(imageFile, TextureFilerType::LinearFilter), _spriteShader, glm::vec2(186.0f, 5.0f), glm::vec2(78.0f, 78.0f)) ));
	_spriteGamepadMapping.insert(std::pair<Gamepad::Button, Sprite*>(Gamepad::Button::Square, new Sprite("_control4", TextureManager::Instance()->LoadFromFile(imageFile, TextureFilerType::LinearFilter), _spriteShader, glm::vec2(273.0f, 5.0f), glm::vec2(78.0f, 78.0f)) ));

	//left,right...
	_spriteGamepadMapping.insert(std::pair<Gamepad::Button, Sprite*>(Gamepad::Button::Left, new Sprite("_control5", TextureManager::Instance()->LoadFromFile(imageFile, TextureFilerType::LinearFilter), _spriteShader, glm::vec2(313.0f, 88.0f), glm::vec2(88.0f, 68.0f)) ));
	_spriteGamepadMapping.insert(std::pair<Gamepad::Button, Sprite*>(Gamepad::Button::Right, new Sprite("_control6", TextureManager::Instance()->LoadFromFile(imageFile, TextureFilerType::LinearFilter), _spriteShader, glm::vec2(413, 82.0f), glm::vec2(88.0f, 68.0f)) ));
	_spriteGamepadMapping.insert(std::pair<Gamepad::Button, Sprite*>(Gamepad::Button::Up, new Sprite("_control7", TextureManager::Instance()->LoadFromFile(imageFile, TextureFilerType::LinearFilter), _spriteShader, glm::vec2(159.0f, 94.0f), glm::vec2(68.0f, 88.0f)) ));
	_spriteGamepadMapping.insert(std::pair<Gamepad::Button, Sprite*>(Gamepad::Button::Down, new Sprite("_control8", TextureManager::Instance()->LoadFromFile(imageFile, TextureFilerType::LinearFilter), _spriteShader, glm::vec2(239.0f, 94.0f), glm::vec2(68.0f, 88.0f)) ));

	_spriteGamepadMapping.insert(std::pair<Gamepad::Button, Sprite*>(Gamepad::Button::LTrigger, new Sprite("_control9", TextureManager::Instance()->LoadFromFile(imageFile, TextureFilerType::LinearFilter), _spriteShader, glm::vec2(6.0f, 87.0f), glm::vec2(128.0f, 60.0f)) ));
	_spriteGamepadMapping.insert(std::pair<Gamepad::Button, Sprite*>(Gamepad::Button::RTrigger, new Sprite("_control10", TextureManager::Instance()->LoadFromFile(imageFile, TextureFilerType::LinearFilter), _spriteShader, glm::vec2(369.0f, 11.0f), glm::vec2(128.0f, 60.0f)) ));

	_spriteGamepadMapping.insert(std::pair<Gamepad::Button, Sprite*>(Gamepad::Button::Select, new Sprite("_control11", TextureManager::Instance()->LoadFromFile(imageFile, TextureFilerType::LinearFilter), _spriteShader, glm::vec2(159.0f, 193.0f), glm::vec2(74.0f, 38.0f)) ));
	_spriteGamepadMapping.insert(std::pair<Gamepad::Button, Sprite*>(Gamepad::Button::Start, new Sprite("_control12", TextureManager::Instance()->LoadFromFile(imageFile, TextureFilerType::LinearFilter), _spriteShader, glm::vec2(239.0f, 194.0f), glm::vec2(74.0f, 38.0f)) ));

	_spriteGamepadMapping.insert(std::pair<Gamepad::Button, Sprite*>(Gamepad::Button::LAnalogLeft, new Sprite("_control13", TextureManager::Instance()->LoadFromFile(imageFile, TextureFilerType::LinearFilter), _spriteShader, glm::vec2(3.0f, 248.0f), glm::vec2(116.0f, 116.0f)) ));
	_spriteGamepadMapping.insert(std::pair<Gamepad::Button, Sprite*>(Gamepad::Button::LAnalogRight, new Sprite("_control14", TextureManager::Instance()->LoadFromFile(imageFile, TextureFilerType::LinearFilter), _spriteShader, glm::vec2(121.0f, 248.0f), glm::vec2(116.0f, 116.0f)) ));
	_spriteGamepadMapping.insert(std::pair<Gamepad::Button, Sprite*>(Gamepad::Button::LAnalogUp, new Sprite("_control15", TextureManager::Instance()->LoadFromFile(imageFile, TextureFilerType::LinearFilter), _spriteShader, glm::vec2(242.0f, 248.0f), glm::vec2(116.0f, 116.0f)) ));
	_spriteGamepadMapping.insert(std::pair<Gamepad::Button, Sprite*>(Gamepad::Button::LAnalogDown, new Sprite("_control16", TextureManager::Instance()->LoadFromFile(imageFile, TextureFilerType::LinearFilter), _spriteShader, glm::vec2(361.0f, 248.0f), glm::vec2(116.0f, 116.0f)) ));

	_spriteGamepadMapping.insert(std::pair<Gamepad::Button, Sprite*>(Gamepad::Button::LAnalogPushed, new Sprite("_control16", TextureManager::Instance()->LoadFromFile(imageFile, TextureFilerType::LinearFilter), _spriteShader, glm::vec2(63.0f, 152.0f), glm::vec2(89.0f, 89.0f)) ));
	_spriteGamepadMapping.insert(std::pair<Gamepad::Button, Sprite*>(Gamepad::Button::RAnalogPushed, new Sprite("_control16", TextureManager::Instance()->LoadFromFile(imageFile, TextureFilerType::LinearFilter), _spriteShader, glm::vec2(377.0f, 155.0f), glm::vec2(89.0f, 89.0f)) ));

	_spriteGamepadMapping.insert(std::pair<Gamepad::Button, Sprite*>(Gamepad::Button::RAnalogLeft, new Sprite("_control13", TextureManager::Instance()->LoadFromFile(imageFile, TextureFilerType::LinearFilter), _spriteShader, glm::vec2(3.0f, 369.0f), glm::vec2(116.0f, 116.0f))));
	_spriteGamepadMapping.insert(std::pair<Gamepad::Button, Sprite*>(Gamepad::Button::RAnalogRight, new Sprite("_control14", TextureManager::Instance()->LoadFromFile(imageFile, TextureFilerType::LinearFilter), _spriteShader, glm::vec2(122.0f, 369.0f), glm::vec2(116.0f, 116.0f))));
	_spriteGamepadMapping.insert(std::pair<Gamepad::Button, Sprite*>(Gamepad::Button::RAnalogUp, new Sprite("_control15", TextureManager::Instance()->LoadFromFile(imageFile, TextureFilerType::LinearFilter), _spriteShader, glm::vec2(242.0f, 369.0f), glm::vec2(116.0f, 116.0f))));
	_spriteGamepadMapping.insert(std::pair<Gamepad::Button, Sprite*>(Gamepad::Button::RAnalogDown, new Sprite("_control16", TextureManager::Instance()->LoadFromFile(imageFile, TextureFilerType::LinearFilter), _spriteShader, glm::vec2(361.0f, 369.0f), glm::vec2(116.0f, 116.0f))));

	_spriteGamepadMapping.insert(std::pair<Gamepad::Button, Sprite*>(Gamepad::Button::ZLTrigger, new Sprite("_control17", TextureManager::Instance()->LoadFromFile(imageFile, TextureFilerType::LinearFilter), _spriteShader, glm::vec2(5.0f, 490.0f), glm::vec2(90.0f, 50.0f))));
	_spriteGamepadMapping.insert(std::pair<Gamepad::Button, Sprite*>(Gamepad::Button::ZRTrigger, new Sprite("_control18", TextureManager::Instance()->LoadFromFile(imageFile, TextureFilerType::LinearFilter), _spriteShader, glm::vec2(105.0f, 490.0f), glm::vec2(90.0f, 50.0f))));

	_spriteGamepadMapping[Gamepad::Button::ZLTrigger]->SetScale(glm::vec2(0.6f, 0.6f));
	_spriteGamepadMapping[Gamepad::Button::ZRTrigger]->SetScale(glm::vec2(0.6f, 0.6f));

	_spriteGamepadMapping[Gamepad::Button::Triangle]->SetScale(glm::vec2(0.4f, 0.4f));
	_spriteGamepadMapping[Gamepad::Button::Circle]->SetScale(glm::vec2(0.4f, 0.4f));
	_spriteGamepadMapping[Gamepad::Button::Cross]->SetScale(glm::vec2(0.4f, 0.4f));
	_spriteGamepadMapping[Gamepad::Button::Square]->SetScale(glm::vec2(0.4f, 0.4f));

	_spriteGamepadMapping[Gamepad::Button::Left]->SetScale(glm::vec2(0.4f, 0.4f));
	_spriteGamepadMapping[Gamepad::Button::Right]->SetScale(glm::vec2(0.4f, 0.4f));
	_spriteGamepadMapping[Gamepad::Button::Up]->SetScale(glm::vec2(0.4f, 0.4f));
	_spriteGamepadMapping[Gamepad::Button::Down]->SetScale(glm::vec2(0.4f, 0.4f));

	_spriteGamepadMapping[Gamepad::Button::LTrigger]->SetScale(glm::vec2(0.5f, 0.5f));
	_spriteGamepadMapping[Gamepad::Button::RTrigger]->SetScale(glm::vec2(0.5f, 0.5f));

	_spriteGamepadMapping[Gamepad::Button::LAnalogPushed]->SetScale(glm::vec2(0.4f, 0.4f));
	_spriteGamepadMapping[Gamepad::Button::RAnalogPushed]->SetScale(glm::vec2(0.4f, 0.4f));

	_spriteGamepadMapping[Gamepad::Button::LAnalogLeft]->SetScale(glm::vec2(0.35f, 0.35f));
	_spriteGamepadMapping[Gamepad::Button::LAnalogRight]->SetScale(glm::vec2(0.35f, 0.35f));
	_spriteGamepadMapping[Gamepad::Button::LAnalogUp]->SetScale(glm::vec2(0.35f, 0.35f));
	_spriteGamepadMapping[Gamepad::Button::LAnalogDown]->SetScale(glm::vec2(0.35f, 0.35f));

	_spriteGamepadMapping[Gamepad::Button::RAnalogLeft]->SetScale(glm::vec2(0.35f, 0.35f));
	_spriteGamepadMapping[Gamepad::Button::RAnalogRight]->SetScale(glm::vec2(0.35f, 0.35f));
	_spriteGamepadMapping[Gamepad::Button::RAnalogUp]->SetScale(glm::vec2(0.35f, 0.35f));
	_spriteGamepadMapping[Gamepad::Button::RAnalogDown]->SetScale(glm::vec2(0.35f, 0.35f));
}

InputHelper::InputHelper()
{
	_inputManager = InputManager::Instance();

	_gamepad = _inputManager->GetGamepadDevice(0);
	_touchDevice = _inputManager->GetTouchDevice(0);

	for (size_t i = 0; i < CarAction::Count; i++)
	{
		_carActionPressed.insert(std::pair<CarAction, bool>(static_cast<CarAction>(i), false));
	}

	for (size_t i = 0; i < MenuAction::MenuCount; i++)
	{
		_menuActionPressed.insert(std::pair<MenuAction, bool>(static_cast<MenuAction>(i), false));
	}

	PrepareGamepadMapping();
	PrepareSpriteGamepadMapping();

	//try load settings
	LoadSettings();

	_updateAll = false;
	_touchHeld = false;
}

InputHelper* InputHelper::Instance()
{
	if (_inputHelper == 0)
	{
		_inputHelper = new InputHelper();
	}

	return _inputHelper;
}

InputHelper::~InputHelper()
{
	{
		std::map<Gamepad::Button, Sprite*>::iterator iter;

		for (iter = _spriteGamepadMapping.begin(); iter != _spriteGamepadMapping.end(); ++iter)
		{
			delete iter->second;
		}

		_spriteGamepadMapping.clear();
	}
}

bool InputHelper::IsMapped(Gamepad::Button button)
{
	for (std::map<CarAction, Gamepad::Button>::iterator i = _carGamepadMapping.begin(); i != _carGamepadMapping.end(); ++i)
	{
		if (i->second == button)
		{
			return true;
		}
	}

	return false;
}

bool InputHelper::ActionHold(CarAction action)
{
	if (_gamepad->KeyDown(_carGamepadMapping[action]))
	{
		return true;
	}

	std::vector<Gamepad::Button>& aliases = _carAliases[action];
	for (size_t i = 0; i < aliases.size(); i++)
	{
		if (_gamepad->KeyDown(aliases[i]) && !IsMapped(aliases[i]))
		{
			return true;
		}
	}

	return false;
}

bool InputHelper::ActionPressed(CarAction action)
{
	if (ActionHold(action) && !_carActionPressed[action])
	{
		_carActionPressed[action] = true;
		return true;
	}

	return false;
}

bool InputHelper::MenuActionHold(MenuAction action)
{
	if (_gamepad->KeyDown(_menuGamepadMapping[action]))
	{
		//a stick held diagonally only counts as its main direction
		bool horizontal = fabsf(_gamepad->LeftAnalogX()) > fabsf(_gamepad->LeftAnalogY());

		switch (_menuGamepadMapping[action])
		{
		case Gamepad::Button::LAnalogLeft:
		case Gamepad::Button::LAnalogRight:
			if (horizontal)
				return true;
			break;
		case Gamepad::Button::LAnalogUp:
		case Gamepad::Button::LAnalogDown:
			if (!horizontal)
				return true;
			break;
		default:
			return true;
		}
	}

	std::vector<Gamepad::Button>& aliases = _menuAliases[action];
	for (size_t i = 0; i < aliases.size(); i++)
	{
		if (_gamepad->KeyDown(aliases[i]))
		{
			return true;
		}
	}

	return false;
}

bool InputHelper::IsRepeating(MenuAction action)
{
	return action <= MenuAction::GridRight || action == MenuAction::PagePrevious || action == MenuAction::PageNext
		|| action == MenuAction::SkinPrevious || action == MenuAction::SkinNext;
}

bool InputHelper::MenuActionPressed(MenuAction action)
{
	if (!MenuActionHold(action))
	{
		return false;
	}

	if (!_menuActionPressed[action])
	{
		_menuActionPressed[action] = true;
		_menuRepeat[action] = Now() + REPEAT_DELAY;
		return true;
	}

	if (IsRepeating(action) && Now() >= _menuRepeat[action])
	{
		_menuRepeat[action] = Now() + REPEAT_INTERVAL;
		return true;
	}

	return false;
}

bool InputHelper::SkipPressed()
{
	glm::vec2 point;

	return MenuActionPressed(MenuAction::MenuAccept) || MenuActionPressed(MenuAction::MenuBack)
		|| MenuActionPressed(MenuAction::MenuExit) || TouchPressed(point);
}

bool InputHelper::StickDrives(CarAction negative, CarAction positive, Gamepad::Button stickNegative, Gamepad::Button stickPositive)
{
	//a stick direction drives unless it is mapped to another action
	return (!IsMapped(stickNegative) || _carGamepadMapping[negative] == stickNegative)
		&& (!IsMapped(stickPositive) || _carGamepadMapping[positive] == stickPositive);
}

float InputHelper::CarAxis(CarAction negative, CarAction positive, Gamepad::Button stickNegative, Gamepad::Button stickPositive, float stick)
{
	if (StickDrives(negative, positive, stickNegative, stickPositive))
	{
		float value = StickValue(stick);
		if (value != 0.0f)
		{
			return value;
		}
	}

	return (ActionHold(positive) ? 1.0f : 0.0f) - (ActionHold(negative) ? 1.0f : 0.0f);
}

float InputHelper::DriveAxis()
{
	return CarAxis(CarAction::Left, CarAction::Right, Gamepad::Button::LAnalogLeft, Gamepad::Button::LAnalogRight, _gamepad->LeftAnalogX());
}

float InputHelper::RotateAxis()
{
	return CarAxis(CarAction::RotateLeft, CarAction::RotateRight, Gamepad::Button::RAnalogLeft, Gamepad::Button::RAnalogRight, _gamepad->RightAnalogX());
}

float InputHelper::StickDriveAxis()
{
	return StickValue(_gamepad->LeftAnalogX());
}

float InputHelper::StickRotateAxis()
{
	return StickValue(_gamepad->RightAnalogX());
}

int InputHelper::GetTouchCount()
{
	return _touchDevice->GetTouchCount();
}

glm::vec2 InputHelper::GetTouchPosition(int touch)
{
	return glm::vec2(_touchDevice->GetTouchX(touch), _touchDevice->GetTouchY(touch));
}

bool InputHelper::TouchPressed(glm::vec2& position)
{
	if (_touchHeld || _touchDevice->GetTouchCount() == 0)
	{
		return false;
	}

	_touchHeld = true;
	position = GetTouchPosition(0);
	return true;
}

void InputHelper::UpdateAllInputs(bool state)
{
	_updateAll = state;
}

void InputHelper::Update()
{
	for (size_t i = 0; i < CarAction::Count; i++)
	{
		CarAction action = static_cast<CarAction>(i);
		_carActionPressed[action] = ActionHold(action);
	}

	double now = Now();

	for (size_t i = 0; i < MenuAction::MenuCount; i++)
	{
		MenuAction action = static_cast<MenuAction>(i);
		_menuActionPressed[action] = MenuActionHold(action);

		//nobody used the repeat, so this screen does not read it: don't repeat right away on the next one
		if (now >= _menuRepeat[action])
		{
			_menuRepeat[action] = now + REPEAT_DELAY;
		}
	}

	_touchHeld = _touchDevice->GetTouchCount() > 0;

	if (_updateAll)
	{
		for (int key = 0; key < Gamepad::Button::Count; key++)
		{
			Gamepad::Button button = static_cast<Gamepad::Button>(key);
			if (_gamepad->KeyUp(button))
			{
				_buttonPressed[button] = false;
			}
			else
			{
				_buttonPressed[button] = true;
			}
		}
	}
}

BindResult InputHelper::ChangeInputForAction(CarAction action)
{
	for (int key = 0; key < Gamepad::Button::Count; key++)
	{
		Gamepad::Button button = static_cast<Gamepad::Button>(key);

		if (!_gamepad->KeyDown(button) || _buttonPressed[button])
		{
			continue;
		}

		//these two open the pause menu
		if (button == Gamepad::Button::Start || button == Gamepad::Button::Select)
		{
			_updateAll = false;
			return BindCancelled;
		}

		//an action that already uses this button gets the old one
		for (std::map<CarAction, Gamepad::Button>::iterator i = _carGamepadMapping.begin(); i != _carGamepadMapping.end(); ++i)
		{
			if (i->second == button)
			{
				i->second = _carGamepadMapping[action];
			}
		}

		_carGamepadMapping[action] = button;
		_updateAll = false;
		return BindChanged;
	}

	return BindWaiting;
}

void InputHelper::ResetCarControls()
{
	PrepareDefaultCarMapping();
}

Sprite* InputHelper::CarActionSprite(CarAction action)
{
	return _spriteGamepadMapping[_carGamepadMapping[action]];
}

Sprite* InputHelper::MenuActionSprite(MenuAction action)
{
	return _spriteGamepadMapping[_menuGamepadMapping[action]];
}

Gamepad::Button InputHelper::CarActionButton(CarAction action)
{
	return _carGamepadMapping[action];
}

Gamepad::Button InputHelper::MenuActionButton(MenuAction action)
{
	return _menuGamepadMapping[action];
}

glm::vec2 InputHelper::TouchToScreen(glm::vec4 screenBound, glm::vec2 touch)
{
	float width = fabsf(screenBound.x) + fabsf(screenBound.y);
	float widthFactor = width / (float)RenderManager::Instance()->GetWidth();
	float dragX = (touch.x * widthFactor) + screenBound.x;

	float height = fabsf(screenBound.w) + fabsf(screenBound.z);
	float heightFactor = height / (float)RenderManager::Instance()->GetHeight();
	
	float dragY = 0.0f;

	if(screenBound.w < screenBound.z)
		dragY = (touch.y * heightFactor) + screenBound.w;
	else
	{
		touch.y = (float)RenderManager::Instance()->GetHeight() - touch.y;
		dragY = (touch.y * heightFactor) + screenBound.z;
	}

	return glm::vec2(dragX, dragY);
}

std::map<int, int> InputHelper::GetActionKeyMapping()
{
	std::map<int, int> _actionKeyMapping;

	for (size_t i = 0; i < CarAction::Count; i++)
	{
		CarAction action = static_cast<CarAction>(i);
		_actionKeyMapping.insert(std::pair<int, int>(i, _carGamepadMapping[action]));
	}

	return _actionKeyMapping;
}

void InputHelper::SetActionKeyMapping(std::map<int, int> mapping)
{
	typedef std::map<int, int>::iterator it_type;
	for (it_type iterator = mapping.begin(); iterator != mapping.end(); iterator++)
	{
		if (iterator->first < 0 || iterator->first >= CarAction::Count || iterator->second < 0 || iterator->second >= Gamepad::Button::Count)
		{
			continue;
		}

		CarAction action = static_cast<CarAction>(iterator->first);
		_carGamepadMapping[action] = static_cast<Gamepad::Button>(iterator->second);
	}
}

void InputHelper::LoadSettings()
{
	std::string fileName = "JellyKeySettings.xml";

	//loac main level file
	AFS::BaseFile* file = AFS::FileManager::Instance()->GetFile(fileName, true);

	if (file == 0)
		return;

	file->Open(AFS::Read, AFS::Binary);

	if (!file->Exist())
	{
		delete file;
		return;
	}

	int dataSize = 0;
	unsigned char* _buffer = file->GetData(dataSize);

	file->Close();
	delete file;

	int bodyNumber = 0;

	//load object
	TiXmlDocument doc;
	if (!doc.LoadContent(_buffer, dataSize))
	{
		return;
	}

	TiXmlHandle hDoc(&doc);
	TiXmlElement* pElem;
	TiXmlHandle hRoot(0);

	std::map<int, int> actionKeyMapping;

	TiXmlElement* ObjectNode = pElem = hDoc.FirstChild("Settings").FirstChild().Element();
	for (ObjectNode; ObjectNode; ObjectNode = ObjectNode->NextSiblingElement())
	{
		int action = atof(ObjectNode->Attribute("action"));
		int key = atof(ObjectNode->Attribute("key"));

		actionKeyMapping.insert(std::pair<int, int>(action, key));
	}

	//replace defaults that changed since the file was saved
	TiXmlElement* settings = hDoc.FirstChild("Settings").Element();
	int version = 1;
	if (settings != 0)
	{
		settings->QueryIntAttribute("version", &version);
	}

	if (version < 2)
	{
		RemoveMapping(actionKeyMapping, CarAction::Left, Gamepad::Button::Left, CarAction::Right, Gamepad::Button::Right);
	}

	if (version == 2)
	{
		RemoveMapping(actionKeyMapping, CarAction::RotateLeft, Gamepad::Button::RAnalogLeft, CarAction::RotateRight, Gamepad::Button::RAnalogRight);
	}

	SetActionKeyMapping(actionKeyMapping);
}

void InputHelper::SaveSettings()
{
	std::string fileName = "JellyKeySettings.xml";

	std::string saveFile = AFS::FileManager::Instance()->GetSaveDirPath() + fileName;

	TiXmlDocument doc;
	TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "", "");
	doc.LinkEndChild(decl);

	//root
	TiXmlElement * root = new TiXmlElement("Settings");
	root->SetAttribute("version", 3);
	doc.LinkEndChild(root);

	//get mapping
	std::map<int, int> actionKeyMapping = GetActionKeyMapping();

	typedef std::map<int, int>::iterator it_type;
	for (it_type iterator = actionKeyMapping.begin(); iterator != actionKeyMapping.end(); iterator++)
	{
		TiXmlElement * cxn = new TiXmlElement("ActionMapping");
		root->LinkEndChild(cxn);
		cxn->SetDoubleAttribute("action", iterator->first);
		cxn->SetDoubleAttribute("key", iterator->second);
	}

	doc.SaveFile(saveFile.c_str());
}
