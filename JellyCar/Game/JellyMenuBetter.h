#ifndef JellyMenuBetter_H
#define JellyMenuBetter_H

#include <Andromeda/System/GameState.h>
#include <Andromeda/System/GameManager.h>

#include <Andromeda/Graphics/RenderManager.h>
#include <Andromeda/Graphics/ShaderManager.h>
#include <Andromeda/Graphics/Shader.h>
#include <Andromeda/Graphics/TextureManager.h>
#include <Andromeda/Graphics/Sprite.h>

#include <Andromeda/Graphics/FrameBufferObject.h>

#include <Andromeda/Input/InputManager.h>

#include <Andromeda/Graphics/Text/TextureAtlas.h>
#include <Andromeda/Graphics/Text/TexturedFont.h>

#include <Andromeda/System/Timer.h>

#include "JellyPhysics/JellyPhysics.h"

#include "../Car/Car.h"
#include "../Levels/LevelManager.h"
#include "../Levels/LevelSoftBody.h"

#include "../Utils/InputHelper.h"
#include "../Utils/AudioHelper.h"
#include "../Utils/TouchTargets.h"
#include "JellyCore.h"
#include "JellyGame.h"
#include "JellyOptions.h"


using namespace Andromeda::System;
using namespace Andromeda::Graphics;
using namespace Andromeda::Input;
using namespace JellyPhysics;



class JellyMenuBetter : public GameState
{
private:

	//what a touch can hit, the number of a level is added to the last one
	enum TouchId
	{
		TouchPlay,
		TouchOptions,
		TouchExit,
		TouchPagePrevious,
		TouchPageNext,
		TouchSkinPrevious,
		TouchSkinNext,
		TouchExitYes,
		TouchExitNo,
		TouchLevel = 100
	};

	JellyCore* _core;

	//render manager
	RenderManager* _renderManager;

	//shader manager
	ShaderManager* _shaderManager;

	//texture manager
	TextureManager* _textureManager;

	//font
	TexturedFont* _menuFont;
	TexturedFont* _smallFont;
	TexturedFont* _titleFont;

	//shader
	Shader* _shader;
	Sprite* _backSprite;
	Sprite* _levelImage;
	Sprite* _boxSprite;

	glm::mat4 _projection;
	glm::mat4 _jellyProjection;

	//timer
	Timer* _timer;
	float _dt;

	//input
	InputHelper* _inputHelper;

	//audio
	AudioHelper* _audioHelper;
	
	//level data
	LevelManager* _levelManager;
	
	//level grid menu  (4 cols x 3 rows per page)
	static const int GRID_COLS = 4;
	static const int GRID_ROWS = 3;
	static const int PAGE_SIZE = GRID_COLS * GRID_ROWS; // 12

	int _gridCol;       // 0..GRID_COLS-1
	int _gridRow;       // 0..GRID_ROWS-1
	int _currentPage;   // 0-based page index
	int _totalPages;

	std::vector<std::string> _sceneFiles;

	// derived helper
	int CurrentIndex() const { return _currentPage * PAGE_SIZE + _gridRow * GRID_COLS + _gridCol; }

	//car skins names
	int carcurrentPosition;
	int carcolumnStartPosition;
	int carpositionsInColumn;
	std::vector<SkinInfo> _carSkins;

	//
	//jelly physics
	World* _world;

	//car
	Car* _car;

	//level objects
	std::vector<LevelSoftBody*> _gameBodies;

	JellyGame* _jellyGame;
	JellyOptions* _jellyOptions;

	//start the selected level again after leaving one
	bool _launchLevel;

	//exit question
	bool _confirmExit;
	bool _confirmYes;

	TouchTargets _touch;

	//selection box of the new menu, moves to the selected cell
	float _cursorX;
	float _cursorY;
	bool _cursorPlaced;

	float _focusPulse;
	float _focusPhase;

	void SelectLevel(int index);
	void MoveHorizontal(int step);
	void MoveVertical(int step);
	void ChangePage(int step);
	int LastRowOnPage();

	void ChangeSkin(int step);
	void UpdateCarSkin();

	void LaunchLevel(GameManager* manager);
	void OpenOptions(GameManager* manager);
	void HandleConfirmExit(GameManager* manager, int touched);

public:

	JellyMenuBetter(JellyCore* core);

	void Init();
	void Enter();
	void CleanUp();

	void Pause();
	void Resume();

	void GamePause();
	void GameResume();

	void HandleEvents(GameManager* manager);
	void Update(GameManager* manager);
	void Draw(GameManager* manager);
};

#endif