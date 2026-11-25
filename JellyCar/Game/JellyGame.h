#ifndef JellyGame_H
#define JellyGame_H

#include <Andromeda/System/GameState.h>
#include <Andromeda/System/GameManager.h>
#include <Andromeda/System/Timer.h>

#include <Andromeda/Graphics/RenderManager.h>
#include <Andromeda/Graphics/ShaderManager.h>
#include <Andromeda/Graphics/Shader.h>
#include <Andromeda/Graphics/TextureManager.h>
#include <Andromeda/Graphics/Sprite.h>
#include <Andromeda/Graphics/Text/TexturedFont.h>
#include <Andromeda/Graphics/FrameBufferObject.h>

#include <Andromeda/Input/InputManager.h>

#include "JellyPhysics/JellyPhysics.h"

#include "../Car/Car.h"
#include "../Levels/LevelManager.h"
#include "../Levels/LevelSoftBody.h"

#include "../Utils/InputHelper.h"
#include "../Utils/AudioHelper.h"
#include "../Utils/TouchTargets.h"

#include "JellyOptions.h"

#include "JellyCore.h"


using namespace Andromeda::System;
using namespace Andromeda::Graphics;
using namespace Andromeda::Input;
using namespace JellyPhysics;

enum GamePlayState
{
	Play,
	Paused,
	Finish,
	OutOfBounds,
	CarBroken
};

//entries of the pause and end of level menus
enum OverlayItem
{
	OverlayResume,
	OverlayCheckpoint,
	OverlayNextLevel,
	OverlayRetry,
	OverlayOptions,
	OverlayLevelSelect
};

//what the menu does when the level is left
enum GameResult
{
	ResultLevelSelect,
	ResultRetry,
	ResultNextLevel
};

class JellyGame : public GameState, JellyPhysics::CollisionCallback
{
private:

	JellyCore* _core;

	//render manager
	RenderManager* _renderManager;

	//shader manager
	ShaderManager* _shaderManager;

	//texture manager
	TextureManager* _textureManager;

	//shader
	Shader* _shader;

	Sprite* _targetSprite;
	Sprite* _backSprite;

	Sprite* _tireSprite;
	Sprite* _ballonSprite;


	//transform_meter
	Sprite* _transformMeter;

	TexturedFont* _menuFont;

	glm::mat4 _projection;
	glm::mat4 _jellyProjection;

	//timer
	Timer* _timer;
	float _dt;
	float _time;

	//input
	InputHelper* _inputHelper;

	//audio
	AudioHelper* _audioHelper;

	//game
	GamePlayState _gamePlayState;

	//jelly physics
	World *mWorld;

	//car
	Car* _car;
	bool _fastCar;
	bool _slowCar;


	//level data
	LevelManager* _levelManager;
	LevelManager* _menuLevelManager;
	std::string _levelName;
	std::string _sceneFile;

	//level objects
	std::vector<LevelSoftBody*> _gameBodies;

	int carBreakCount;

	bool _showMap;

	Vector2 _wholeMapSize;
	Vector2 _wholeMapPosition;
	AABB _worldLimits;
	AABB _mapLimits;

	Vector2 _levelTarget;
	float _levelLine;

	//transformeter
	float				mTransformMeter;
	float				mTransformMeterRechargeSpeed;
	int					mTransformMeterGrowDir;
	int					mTransformTex;

	//new records
	bool _newTimeRecord;
	bool _newJumpRecord;

	//hit sound
	float _hitTime;
	float _chassisHit;

	//jumping
	bool _isJumping;
	bool _inTheAir;
	float _jumpStartPosition;
	float _bestJumpLenght;

	//checkpoint
	bool _checkpoint;
	glm::vec2 _checkpointPosition;

	//ballon and tire
	bool _haveBallon;
	bool _haveTire;

	bool _ballonActive;
	bool _tireActive;


	glm::vec4 _screenBounds;

	AABB _ballonAABB;
	AABB _tireAABB;

	float _ballonTime;
	float _tireTime;

	bool initialized;

	//pause and end of level menu
	std::vector<OverlayItem> _overlayItems;
	int _overlaySelected;
	float _overlayTime;
	TouchTargets _touch;

	bool _nextLevelAvailable;
	GameResult _result;

    JellyOptions* _jellyOptions;
	
private:

	void UpdateTransformMeter(float dt);

	void UpdateProjection();

	void OpenOverlay(GamePlayState state);
	void HandleOverlay(GameManager* manager);
	void SelectOverlayItem(GameManager* manager, OverlayItem item);
	std::string OverlayLabel(OverlayItem item);
	std::string OverlayTitle();

	void ResumePlay();
	void RespawnAtCheckpoint();
	void FinishLevel();
	void LeaveLevel(GameManager* manager, GameResult result);

	void ToggleBallon();
	void ToggleTire();

	//power up HUD, slots run left to right from the bottom left corner
	void PowerUpSlot(int slot, int& x, int& y);
	AABB PowerUpTouchArea(int slot);
	void DrawPowerUp(int slot, Sprite* icon, CarAction action, float timeLeft, bool active);
	void DrawPowerUps();

public:

	JellyGame(JellyCore* core);

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

	//pause and end of level screens, implemented in Menus/New and Menus/Old
	void DrawOverlay();

	bool LoadLevel(LevelManager* levelManager,std::string levelName,std::string sceneName, std::string carFileName);

	void SetNextLevelAvailable(bool available);
	GameResult GetResult();

	void SetChassisTextures(Texture* small, Texture* big);
	void SetTireTextures(Texture* small, Texture* big);

	bool collisionFilter(Body* bA, int bodyApm, Body* bodyB, int bodyBpm1, int bodyBpm2, Vector2 hitPt, float normalVel);
};

#endif
