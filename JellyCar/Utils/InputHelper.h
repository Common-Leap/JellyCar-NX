#ifndef InputHelper_H
#define InputHelper_H

#include <Andromeda/Input/InputManager.h>

using namespace Andromeda::Input;

#include <Andromeda/Graphics/Sprite.h>
using namespace Andromeda::Graphics;

#include <map>
#include <vector>
#include <string>

enum CarAction
{
	Left = 0,
	Right = 1,
	RotateLeft = 2,
	RotateRight = 3,
	Transform = 4,
	Ballon = 5,
	Tire = 6,
	Map = 7,
	Count = 8
};

enum MenuAction
{
	//lists and options: left stick or d-pad
	MenuUp,
	MenuDown,
	MenuLeft,
	MenuRight,
	//level grid: d-pad only, the left stick drives the car there
	GridUp,
	GridDown,
	GridLeft,
	GridRight,
	MenuAccept,
	MenuBack,
	MenuPause,
	MenuExit,
	MenuRetry,
	MenuReset,
	PagePrevious,
	PageNext,
	SkinPrevious,
	SkinNext,
	MenuCount
};

enum BindResult
{
	BindWaiting,
	BindChanged,
	BindCancelled
};

class InputHelper
{
private:

	static InputHelper* _inputHelper;

	//input
	InputManager* _inputManager;

	GamepadDevice* _gamepad;
	TouchDevice* _touchDevice;

	bool _updateAll;

	//key state
	std::map<Gamepad::Button, bool> _buttonPressed;

	//buttons that do the same as the mapped one, as long as they are not mapped to something else
	std::map<CarAction, std::vector<Gamepad::Button> > _carAliases;
	std::map<MenuAction, std::vector<Gamepad::Button> > _menuAliases;

	//time of the next repeat of a held direction
	std::map<MenuAction, double> _menuRepeat;

	//touch
	bool _touchHeld;

	//keys mapping
	std::map<CarAction, Gamepad::Button> _carGamepadMapping;
	std::map<MenuAction, Gamepad::Button> _menuGamepadMapping;

	//
	std::map<CarAction, bool> _carActionPressed;
	std::map<MenuAction, bool> _menuActionPressed;

	//sprite mapping
	Shader* _spriteShader;

	std::map<Gamepad::Button, Sprite*> _spriteGamepadMapping;

	void PrepareDefaultCarMapping();
	void PrepareGamepadMapping();
	void PrepareSpriteGamepadMapping();

	InputHelper();

	bool IsMapped(Gamepad::Button button);
	bool MenuActionHold(MenuAction action);
	bool IsRepeating(MenuAction action);

	bool StickDrives(CarAction negative, CarAction positive, Gamepad::Button stickNegative, Gamepad::Button stickPositive);
	float CarAxis(CarAction negative, CarAction positive, Gamepad::Button stickNegative, Gamepad::Button stickPositive, float stick);

public:

	static InputHelper* Instance();
	~InputHelper();

	void Update();
	void UpdateAllInputs(bool state);

	bool ActionPressed(CarAction action);
	bool ActionHold(CarAction action);

	//from -1 to 1, sticks give the values in between
	float DriveAxis();
	float RotateAxis();

	//the same, from the sticks only
	float StickDriveAxis();
	float StickRotateAxis();

	bool MenuActionPressed(MenuAction action);

	//any of the buttons that skip the splash and the intro, or a touch
	bool SkipPressed();

	int GetTouchCount();
	glm::vec2 GetTouchPosition(int touch);

	//true once when the screen is touched
	bool TouchPressed(glm::vec2& position);

	glm::vec2 TouchToScreen(glm::vec4 screenBound,glm::vec2 touch);

	Sprite* CarActionSprite(CarAction action);
	Sprite* MenuActionSprite(MenuAction action);

	Gamepad::Button CarActionButton(CarAction action);
	Gamepad::Button MenuActionButton(MenuAction action);

	BindResult ChangeInputForAction(CarAction action);
	void ResetCarControls();

	std::map<int, int> GetActionKeyMapping();
	void SetActionKeyMapping(std::map<int, int> mapping);

	void LoadSettings();
	void SaveSettings();
};

#endif