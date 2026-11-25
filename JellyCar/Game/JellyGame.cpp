#include "JellyGame.h"

#include "../Utils/JellyHelper.h"
#include "../Utils/RectangleDrawer.h"
#include "../Utils/UiTheme.h"
#include "../Utils/UiLayout.h"
#include "../Utils/UiPrompt.h"

#include <algorithm>
#include <stdio.h>
#include <sstream>


template <typename T>
std::string to_string(const T& value)
{
	std::ostringstream s;
	s << value;
	return s.str();
}

namespace
{
	const int TIRE_SLOT = 0;
	const int BALLON_SLOT = 1;

	const float POWER_UP_TIME = 30.0f;
	const float POWER_UP_LOW_TIME = 5.0f;

	const int SLOT_SIZE = 64;
	const int SLOT_GAP = 14;
	const int SLOT_MARGIN = 24;

	//so that a button held while driving does not pick an entry
	const float OVERLAY_DELAY = 0.35f;

	//longest time one frame can count for
	const float MAX_DELTA = 0.1f;
}

JellyGame::JellyGame(JellyCore* core)
{
	_core = core;

	_renderManager = NULL;
	_shaderManager = NULL;
	_textureManager = NULL;

	_shader = NULL;

	_targetSprite = NULL;
	_backSprite = NULL;
	_tireSprite = NULL;
	_ballonSprite = NULL;

	_transformMeter = NULL;

	_menuFont = NULL;

	_timer = NULL;

	_inputHelper = NULL;
	_audioHelper = NULL;

	mWorld = NULL;
	_car = NULL;

	_levelManager = NULL;
	_menuLevelManager = NULL;

    _jellyOptions = nullptr;

	initialized = false;

	_overlaySelected = 0;
	_overlayTime = 0.0f;

	_nextLevelAvailable = false;
	_result = ResultLevelSelect;
}

void JellyGame::Init()
{
	if (initialized)
	{
		return;
	}

	_renderManager = RenderManager::Instance();
	_shaderManager = ShaderManager::Instance();
	_textureManager = TextureManager::Instance();

	_renderManager->SetClearColor(0xffffffff);

	//font
	_menuFont = _core->_menuFont;

	//load shader
	_shader = _shaderManager->LoadFromFile("sprite", "Assets/Shaders/sprite", "Assets/Shaders/sprite", Textured);

	_projection = glm::ortho(0.0f, (float)_renderManager->GetWidth(), (float)_renderManager->GetHeight(), 0.0f, -1.0f, 1.0f);
	_jellyProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, -1.0f, 1.0f);

	//
	_screenBounds = glm::vec4(0.0f, (float)_renderManager->GetWidth(), (float)_renderManager->GetHeight(), 0.0f);

	//
	_renderManager->SetDepth(false);

	//input
	_inputHelper = InputHelper::Instance();

	//init audio helper
	_audioHelper = AudioHelper::Instance();
	_audioHelper->PlaySlowEngine();

	_fastCar = false;
	_slowCar = true;

	//jelly physics
	mWorld = new World();

	_levelManager = 0;
	//
	_backSprite = new Sprite("paper", "Assets/Jelly/Texture/paper.png", _shader);

	//load target sprite
	_targetSprite = new Sprite("target", "Assets/Jelly/Texture/finish.png", _shader);
	_targetSprite->SetScale(glm::vec2(0.04f, 0.04f));

	_tireSprite = new Sprite("tireSprite", "Assets/Jelly/Texture/tire.png", _shader);
	_tireSprite->SetScale(glm::vec2(0.04f, 0.04f));

	_ballonSprite = new Sprite("ballonSprite", "Assets/Jelly/Texture/ballon.png", _shader);
	_ballonSprite->SetScale(glm::vec2(0.04f, 0.04f));	

	_transformMeter = new Sprite("transformmeter", "Assets/Jelly/Texture/transform_meter.png", _shader);
	_transformMeter->SetPosition(glm::vec2(128,40));

	//menu
	_showMap = false;

	//timer
	_dt = 0.0f;
	_time = 0.0f;
	_timer = new Timer();

	_hitTime = 0.0f;
	_chassisHit = 0.0f;

	//jumping
	_isJumping = false;
	_inTheAir = false;
	_jumpStartPosition = 0.0f;
	_bestJumpLenght = 0.0f;

	mTransformMeter = 1.0f;
	mTransformMeterRechargeSpeed = 1.0f / 13.0f;
	mTransformMeterGrowDir = 0;

	//checkpoint
	_checkpoint = false;

	//ballon and tire initialization
	_haveBallon = false;
	_haveTire = false;

	_ballonActive = false;
	_tireActive = false;

	_ballonTime = 0.0f;
	_tireTime = 0.0f;

	_tireAABB = PowerUpTouchArea(TIRE_SLOT);
	_ballonAABB = PowerUpTouchArea(BALLON_SLOT);

	_gamePlayState = GamePlayState::Play;

	initialized = true;
}

bool JellyGame::LoadLevel(LevelManager* levelManager, std::string levelName,std::string sceneName, std::string carFileName)
{
	_sceneFile = sceneName;
	_levelName = levelName;
	_menuLevelManager = levelManager;

	//level manager
	_levelManager = new LevelManager();
	_levelManager->SetAssetsLocation("Assets/Jelly/");

	//init physic
	_levelManager->InitPhysic(mWorld);

	// filter callbacks (for sound playback)
	mWorld->setMaterialPairFilterCallback(0, 2, this);
	mWorld->setMaterialPairFilterCallback(1, 2, this);
	mWorld->setMaterialPairFilterCallback(4, 2, this);

	mWorld->setMaterialPairFilterCallback(0, 3, this);
	mWorld->setMaterialPairFilterCallback(1, 3, this);
	mWorld->setMaterialPairFilterCallback(4, 3, this);

	//ballon and tire collision
	mWorld->setMaterialPairFilterCallback(5, 2, this);
	mWorld->setMaterialPairFilterCallback(5, 3, this);

	//load level
	//_levelManager->LoadLevel(mWorld, _sceneFile, carFileName);
	_levelManager->LoadCompiledLevel(mWorld, _sceneFile, carFileName);

	//car
	_car = _levelManager->GetCar();

	//level elements
	_gameBodies = _levelManager->GetLevelBodies();

	//set map size
	_wholeMapPosition = _levelManager->GetWorldCenter();
	_wholeMapSize = _levelManager->GetWorldSize();
	_worldLimits = _levelManager->GetWorldLimits();

	//compute map view
	//_mapLimits
	//doing this at 2 am was not so easy :p
	if (_wholeMapSize.X > _wholeMapSize.Y)
	{
		float sizeX = _wholeMapSize.X;
		float sizeY = _wholeMapSize.X  * 0.56f;

		float startX, startY;
		float endX, endY;

		if (sizeY < _wholeMapSize.Y)
		{
			startY = _worldLimits.Min.Y - 5.0f;
			endY = _worldLimits.Max.Y + 5.0f;

			sizeX = ((_wholeMapSize.Y + 10.0f)/ 0.56f);

			startX = _wholeMapPosition.X - (sizeX / 2.0f);
			endX = _wholeMapPosition.X + (sizeX / 2.0f);
		}
		else
		{
			startX = _worldLimits.Min.X;
			endX = _worldLimits.Max.X;

			startY = _wholeMapPosition.Y - (sizeY / 2.0f);
			endY = _wholeMapPosition.Y + (sizeY / 2.0f);
		}

		_mapLimits.Min.X = startX;
		_mapLimits.Min.Y = startY;
		_mapLimits.Max.X = endX;
		_mapLimits.Max.Y = endY;
	}
	else
	{
		float sizeX = _wholeMapSize.Y  / 0.56f;
		float sizeY = _wholeMapSize.Y;

		float startX = 0.0f, startY = 0.0f;
		float endX = 0.0f, endY = 0.0f;

		if (sizeX < _wholeMapSize.X)
		{
			//??
		}
		else
		{
			startY = _worldLimits.Min.Y;
			endY = _worldLimits.Max.Y;

			startX = _wholeMapPosition.X - (sizeX / 2.0f);
			endX = _wholeMapPosition.X + (sizeX / 2.0f);
		}

		_mapLimits.Min.X = startX;
		_mapLimits.Min.Y = startY;
		_mapLimits.Max.X = endX;
		_mapLimits.Max.Y = endY;
	}

	_wholeMapSize.X += 5;
	_wholeMapSize.Y = (_wholeMapSize.X *0.56f) / 2.0f;
	_wholeMapSize.X = _wholeMapSize.X / 2.0f;

	_levelTarget = _levelManager->GetLevelTarget();
	_levelLine = _levelManager->GetLevelLine();

	_targetSprite->SetPosition(glm::vec2(_levelTarget.X, _levelTarget.Y));

	carBreakCount = 0;

	_newTimeRecord = false;
	_newJumpRecord = false;

	_gamePlayState = GamePlayState::Play;

	UpdateProjection();

	//loading does not count
	_timer->GetDelta();

	return true;
}

void JellyGame::SetNextLevelAvailable(bool available)
{
	_nextLevelAvailable = available;
}

GameResult JellyGame::GetResult()
{
	return _result;
}

void JellyGame::SetChassisTextures(Texture* small, Texture* big)
{
	_car->SetChassisTextures(small, big);
}

void JellyGame::SetTireTextures(Texture* small, Texture* big)
{
	_car->SetTireTextures(small, big);
}

bool JellyGame::collisionFilter(Body* bodyA, int bodyApm, Body* bodyB, int bodyBpm1, int bodyBpm2, Vector2 hitPt, float normalVel)
{
	if (bodyB->getMaterial() == 5 || bodyA->getMaterial() == 5)
	{
		Body* itemBody = 0;

		if (bodyB->getMaterial() == 5)
		{
			itemBody = bodyB;
		}
		else
		{
			itemBody = bodyA;
		}

		if (itemBody->getDisable())
		{
			return false;
		}

		if (itemBody->GetName() == "itemstick")
		{
			_haveTire = true;
			_tireTime = POWER_UP_TIME;
		}
		else if(itemBody->GetName() == "itemballoon")
		{
			_haveBallon = true;
			_ballonTime = POWER_UP_TIME;
		}
		else if(itemBody->GetName() == "checkpoint")
		{
			_checkpoint = true;
			_checkpointPosition = glm::vec2(itemBody->mDerivedPos.X, itemBody->mDerivedPos.Y + 1);
		}

		itemBody->setDisable(true);

		return false;
	}

	_isJumping = false;

	//hit sound
	float hitVel = fabsf(normalVel);

	if (hitVel > _chassisHit) 
	{
		_chassisHit = hitVel;
	}

	return true;
}

void JellyGame::Enter()
{

}

void JellyGame::CleanUp()
{
	//remove sprites
	delete _targetSprite;
	delete _backSprite;
	delete _transformMeter;
    delete _tireSprite;
    delete _ballonSprite;

	//remove timer
	delete _timer;

	//delete level manager
	//and all info
	delete _levelManager;

	//remove physic world
	delete mWorld;

	//clear pointers
	_gameBodies.clear();
}

void JellyGame::Pause()
{

}

void JellyGame::Resume()
{
	delete _jellyOptions;
	_jellyOptions = nullptr;

	_inputHelper->Update();
}

void JellyGame::GamePause()
{

}

void JellyGame::GameResume()
{

}

void JellyGame::ToggleBallon()
{
	_ballonActive = !_ballonActive;
	_car->UseBallon(_ballonActive);
}

void JellyGame::ToggleTire()
{
	_tireActive = !_tireActive;
	_car->UseNearestGracity(_tireActive);
}

void JellyGame::ResumePlay()
{
	_gamePlayState = GamePlayState::Play;

	if (_fastCar)
	{
		_audioHelper->PlayFastEngine();
	}
	else
	{
		_audioHelper->PlaySlowEngine();
	}
}

void JellyGame::RespawnAtCheckpoint()
{
	Vector2 pos = Vector2(_checkpointPosition.x, _checkpointPosition.y);
	Vector2 scale = Vector2(1.0f, 1.0f);

	_car->getChassisBody()->setVelocity(Vector2(0, 0));
	_car->getTire(0)->setVelocity(Vector2(0, 0));
	_car->getTire(1)->setVelocity(Vector2(0, 0));

	_car->getChassisBody()->setPositionAngle(pos, 0.0f, scale);
	_car->getTire(0)->setPositionAngle(Vector2(_checkpointPosition.x + 1.5f, _checkpointPosition.y - 0.3f), 0.0f, scale);
	_car->getTire(1)->setPositionAngle(Vector2(_checkpointPosition.x - 1.3f, _checkpointPosition.y - 0.3f), 0.0f, scale);
}

void JellyGame::LeaveLevel(GameManager* manager, GameResult result)
{
	_result = result;

	_audioHelper->StopEngineSound();
	_levelManager->ClearLevel(mWorld);
	manager->PopState();
}

void JellyGame::OpenOverlay(GamePlayState state)
{
	_gamePlayState = state;
	_audioHelper->StopEngineSound();

	_overlayItems.clear();
	_overlaySelected = 0;
	_overlayTime = 0.0f;

	if (state == GamePlayState::Paused)
	{
		_overlayItems.push_back(OverlayResume);

		if (_checkpoint)
		{
			_overlayItems.push_back(OverlayCheckpoint);
		}
	}

	if (state == GamePlayState::Finish && _nextLevelAvailable)
	{
		_overlayItems.push_back(OverlayNextLevel);
	}

	_overlayItems.push_back(OverlayRetry);

	if (state == GamePlayState::Paused)
	{
		_overlayItems.push_back(OverlayOptions);
	}

	_overlayItems.push_back(OverlayLevelSelect);
}

std::string JellyGame::OverlayLabel(OverlayItem item)
{
	switch (item)
	{
	case OverlayResume:     return "Resume";
	case OverlayCheckpoint: return "Go to checkpoint";
	case OverlayNextLevel:  return "Next level";
	case OverlayRetry:      return "Retry";
	case OverlayOptions:    return "Options";
	default:                return "Level select";
	}
}

std::string JellyGame::OverlayTitle()
{
	switch (_gamePlayState)
	{
	case GamePlayState::Finish:      return "Level finished";
	case GamePlayState::CarBroken:   return "Car broken";
	case GamePlayState::OutOfBounds: return "Out of level";
	default:                         return "Pause";
	}
}

void JellyGame::SelectOverlayItem(GameManager* manager, OverlayItem item)
{
	switch (item)
	{
	case OverlayResume:
		ResumePlay();
		break;
	case OverlayCheckpoint:
		RespawnAtCheckpoint();
		ResumePlay();
		break;
	case OverlayNextLevel:
		LeaveLevel(manager, ResultNextLevel);
		break;
	case OverlayRetry:
		LeaveLevel(manager, ResultRetry);
		break;
	case OverlayOptions:
		_jellyOptions = new JellyOptions(_core);
		manager->PushState(_jellyOptions);
		break;
	case OverlayLevelSelect:
		LeaveLevel(manager, ResultLevelSelect);
		break;
	}
}

void JellyGame::HandleOverlay(GameManager* manager)
{
	bool paused = _gamePlayState == GamePlayState::Paused;

	if (!paused && _overlayTime < OVERLAY_DELAY)
	{
		return;
	}

	int count = (int)_overlayItems.size();

	if (_inputHelper->MenuActionPressed(MenuAction::MenuUp))
	{
		_overlaySelected = (_overlaySelected + count - 1) % count;
	}

	if (_inputHelper->MenuActionPressed(MenuAction::MenuDown))
	{
		_overlaySelected = (_overlaySelected + 1) % count;
	}

	glm::vec2 point;
	if (_inputHelper->TouchPressed(point))
	{
		int touched = _touch.Find(point);
		if (touched != TouchTargets::None && touched < count)
		{
			SelectOverlayItem(manager, _overlayItems[touched]);
			return;
		}
	}

	if (_inputHelper->MenuActionPressed(MenuAction::MenuAccept))
	{
		SelectOverlayItem(manager, _overlayItems[_overlaySelected]);
		return;
	}

	if (_inputHelper->MenuActionPressed(MenuAction::MenuRetry))
	{
		SelectOverlayItem(manager, OverlayRetry);
		return;
	}

	if (paused)
	{
		if (_inputHelper->MenuActionPressed(MenuAction::MenuBack)
			|| _inputHelper->MenuActionPressed(MenuAction::MenuPause)
			|| _inputHelper->MenuActionPressed(MenuAction::MenuExit))
		{
			SelectOverlayItem(manager, OverlayResume);
		}
	}
	else if (_inputHelper->MenuActionPressed(MenuAction::MenuBack))
	{
		SelectOverlayItem(manager, OverlayLevelSelect);
	}
}

void JellyGame::HandleEvents(GameManager* manager)
{
	if (_gamePlayState != GamePlayState::Play)
	{
		//may leave the level, which deletes this object
		HandleOverlay(manager);
		InputHelper::Instance()->Update();
		return;
	}

	if (_inputHelper->MenuActionPressed(MenuAction::MenuPause) || _inputHelper->MenuActionPressed(MenuAction::MenuExit))
	{
		OpenOverlay(GamePlayState::Paused);
		_inputHelper->Update();
		return;
	}

	if (_inputHelper->ActionPressed(CarAction::Map))
	{
		_showMap = !_showMap;
	}

	//wheels and chassis
	float drive = _inputHelper->DriveAxis();

	_car->setTorque(drive);
	_car->mChassis->torque = _inputHelper->RotateAxis();

	if (_inputHelper->ActionPressed(CarAction::Transform))
	{
		if (mTransformMeter > 0.0f)
		{
			if (_car->Transform())
				mTransformMeterGrowDir = (mTransformMeterGrowDir >= 0) ? -1 : 1;
		}
	}

	if (_haveBallon && _inputHelper->ActionPressed(CarAction::Ballon))
	{
		ToggleBallon();
	}

	if (_haveTire && _inputHelper->ActionPressed(CarAction::Tire))
	{
		ToggleTire();
	}

	if (drive != 0.0f)
	{
		if (_car->getVelocity().length() > 2.0f && _slowCar)
		{
			_audioHelper->PlayFastEngine();
			_slowCar = false;
			_fastCar = true;
		}
	}
	else if (_fastCar)
	{
		_audioHelper->PlaySlowEngine();
		_slowCar = true;
		_fastCar = false;
	}

	//tire and ballon can be touched
	glm::vec2 point;
	if (!_showMap && _inputHelper->TouchPressed(point))
	{
		glm::vec2 touch = _inputHelper->TouchToScreen(_screenBounds, point);

		if (_haveBallon && _ballonAABB.contains(Vector2(touch.x, touch.y)))
		{
			ToggleBallon();
		}

		if (_haveTire && _tireAABB.contains(Vector2(touch.x, touch.y)))
		{
			ToggleTire();
		}
	}

	_inputHelper->Update();
}

void JellyGame::UpdateTransformMeter(float dt)
{
	// transform meter update.
	if (mTransformMeterGrowDir != 0)
	{
		//float oldValue = mTransformMeter;
		mTransformMeter += (mTransformMeterRechargeSpeed * dt * (float)mTransformMeterGrowDir);

		if (mTransformMeter <= 0.0f)
		{
			if (_car->TransformStatus != Car::Normal)
				_car->Transform();
		}

		if (mTransformMeter <= -0.2f)
		{
			mTransformMeter = -0.2f;
			mTransformMeterGrowDir = 1;
		}

		if ((mTransformMeter > 1.0f))
		{
			mTransformMeter = 1.0f;
			mTransformMeterGrowDir = 0;
		}

	}
}

void JellyGame::UpdateProjection()
{
	//screen ratio 0.56
	if (_showMap)
	{
		_jellyProjection = glm::ortho(_mapLimits.Min.X, _mapLimits.Max.X, _mapLimits.Min.Y, _mapLimits.Max.Y, -1.0f, 1.0f);
	}
	else
	{
		_jellyProjection = glm::ortho(-20.0f + _car->getPosition().X, _car->getPosition().X + 20.0f, -11.2f + _car->getPosition().Y, _car->getPosition().Y + 11.2f, -1.0f, 1.0f);
	}
}

void JellyGame::FinishLevel()
{
	if (_time < _menuLevelManager->GetTime(_levelName))
	{
		_newTimeRecord = true;
		_menuLevelManager->SetTime(_levelName, _time);
	}

	if (_bestJumpLenght > _menuLevelManager->GetJump(_levelName))
	{
		_newJumpRecord = true;
		_menuLevelManager->SetJump(_levelName, _bestJumpLenght);
	}

	if (_newTimeRecord || _newJumpRecord)
	{
		_menuLevelManager->SaveScores("JellyScore.xml");
	}

	OpenOverlay(GamePlayState::Finish);
}

void JellyGame::Update(GameManager* manager)
{
	//read every frame, so that the time spent paused does not count
	_dt = std::min(_timer->GetDelta(), MAX_DELTA);

	if (_gamePlayState != GamePlayState::Play)
	{
		_overlayTime += _dt;
	}

	if (_gamePlayState == GamePlayState::Play)
	{
		_time += _dt;
		_hitTime += _dt;

		//hit info - zero before update/callback event
		_chassisHit = 0;

		//jumping info - true before update/callback event
		_isJumping = true;

		//Update physic
		for (int i = 0; i < 6; i++)
		{
			mWorld->update(0.004f);

			for (size_t i = 0; i < _gameBodies.size(); i++)
				_gameBodies[i]->Update(0.004f);

			_car->clearForces();
			_car->update(0.004f);

			UpdateTransformMeter(0.004f);
		}


		if (_ballonActive)
		{
			_ballonTime -= _dt;

			if (_ballonTime <= 0.0f)
			{
				_car->UseBallon(false);
				_ballonActive = false;
				_haveBallon = false;
			}
		}

		if (_tireActive)
		{
			_tireTime -= _dt;

			if (_tireTime <= 0.0f)
			{
				_car->UseNearestGracity(false);
				_tireActive = false;
				_haveTire = false;
			}
		}

		//update camera
		UpdateProjection();

		//check level ending
		bool finished = _car->getChassisBody()->getAABB().contains(_levelTarget) && _car->getChassisBody()->contains(_levelTarget);

		for (int i = 0; i < 2; i++)
		{
			finished = finished || (_car->getTire(i)->getAABB().contains(_levelTarget) && _car->getTire(i)->contains(_levelTarget));
		}

		if (finished)
		{
			FinishLevel();
			return;
		}

		//check out of bounds
		if (_car->getPosition().Y < _levelLine)
		{
			if (_checkpoint)
			{
				RespawnAtCheckpoint();
			}
			else
			{
				OpenOverlay(GamePlayState::OutOfBounds);
				return;
			}
		}

		//check car broken
		AABB chassisAABB = _car->getChassisBody()->getAABB();
		Vector2 chassisSize = chassisAABB.Max - chassisAABB.Min;
		if ((fabsf(chassisSize.X) > 17.0f) || (fabsf(chassisSize.Y) > 17.0f))
		{
			OpenOverlay(GamePlayState::CarBroken);
			return;
		}

		//car broken
		if (mWorld->getPenetrationCount() > 20)
		{
			carBreakCount++;
			if (carBreakCount > 5)
			{
				OpenOverlay(GamePlayState::CarBroken);
				return;
			}
		}

		//jumping
		if (_isJumping && !_inTheAir)
		{
			//we just started jumping
			_jumpStartPosition = _car->getPosition().X;

			_inTheAir = true;
		}

		if (!_isJumping && _inTheAir)
		{
			//end of jump - check if there is new jump record
			float jumpLenght = fabsf(_jumpStartPosition - _car->getPosition().X);

			if (jumpLenght > _bestJumpLenght)
			{
				_bestJumpLenght = jumpLenght;
			}

			_inTheAir = false;
		}

		//hit sound
		if (_chassisHit > 0.0f)
		{
			if ((_chassisHit > 3.0f) && (_hitTime > 0.3f))
			{
				//play hit sound
				_audioHelper->PlayHitSound();

				//set timer
				_hitTime = 0.0f;
			}
		}

	}	
}

void JellyGame::PowerUpSlot(int slot, int& x, int& y)
{
	x = SLOT_MARGIN + slot * (SLOT_SIZE + SLOT_GAP);
	y = _renderManager->GetHeight() - SLOT_MARGIN - SLOT_SIZE;
}

AABB JellyGame::PowerUpTouchArea(int slot)
{
	int x, y;
	PowerUpSlot(slot, x, y);

	//a little bigger than the slot, without reaching its neighbour
	const int pad = SLOT_GAP / 2;

	AABB area;
	area.expandToInclude(Vector2((float)(x - pad), (float)(y - pad)));
	area.expandToInclude(Vector2((float)(x + SLOT_SIZE + pad), (float)(y + SLOT_SIZE + pad)));
	return area;
}

void JellyGame::DrawPowerUp(int slot, Sprite* icon, CarAction action, float timeLeft, bool active)
{
	RectangleDrawer* ui = RectangleDrawer::Instance();

	int x, y;
	PowerUpSlot(slot, x, y);

	bool low = timeLeft <= POWER_UP_LOW_TIME;
	float pulse = 0.5f + 0.5f * sinf(_time * (low ? 14.0f : 6.0f));

	//in use: glowing green frame, red when about to run out
	if (active)
	{
		glm::vec4 frame = low ? UiTheme::PowerUpLow() : UiTheme::PowerUpActive();
		glm::vec4 glow = frame;
		glow.a = 0.20f + pulse * 0.35f;

		ui->DrawRoundedRect(x - 5, y - 5, SLOT_SIZE + 10, SLOT_SIZE + 10, UiTheme::CellRadius() + 5.0f,
			glow, glm::vec4(0, 0, 0, 0), 0.0f, _projection);
		ui->DrawRoundedRect(x, y, SLOT_SIZE, SLOT_SIZE, UiTheme::CellRadius(),
			UiTheme::PowerUpFillActive(), frame, 2.5f, _projection);
	}
	else
	{
		ui->DrawRoundedRect(x, y, SLOT_SIZE, SLOT_SIZE, UiTheme::CellRadius(),
			UiTheme::CellFill(), UiTheme::CellBorder(), 1.0f, _projection);
	}

	//icon, faded while not in use
	const int barH = 6;
	const int barPad = 9;
	icon->SetScale(glm::vec2(0.31f, 0.31f));
	icon->SetSolor(glm::vec4(1.0f, 1.0f, 1.0f, active ? 1.0f : 0.55f));
	icon->SetPosition(glm::vec2(x + SLOT_SIZE / 2, y + (SLOT_SIZE - barH - barPad) / 2 + 2));
	icon->Draw(_projection);

	//the same sprite marks the item in the level
	icon->SetSolor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

	//time left
	ui->DrawProgressBar(x + barPad, y + SLOT_SIZE - barPad - barH + 2, SLOT_SIZE - barPad * 2, barH, barH / 2.0f,
		timeLeft / POWER_UP_TIME, low, _projection);

	//text is drawn over the pause and finish panels, so only add it during play
	if (_gamePlayState == GamePlayState::Play)
	{
		UiPrompt::DrawButton(ui, _core->_badgeFont, _projection, x + SLOT_SIZE / 2, y - 24,
			_inputHelper->CarActionButton(action), 28);
	}
}

void JellyGame::DrawPowerUps()
{
	if (_showMap)
		return;

	if (_haveTire && _tireTime > 0.0f)
		DrawPowerUp(TIRE_SLOT, _tireSprite, CarAction::Tire, _tireTime, _tireActive);

	if (_haveBallon && _ballonTime > 0.0f)
		DrawPowerUp(BALLON_SLOT, _ballonSprite, CarAction::Ballon, _ballonTime, _ballonActive);
}

void JellyGame::Draw(GameManager* manager)
{
	//_core->frameObject->Bind();

	//start frame
	_renderManager->StartFrame();

	//clear screen
	_renderManager->ClearScreen();

	//render background
	_backSprite->SetScale(glm::vec2(0.1f, 0.1f));

	if (_showMap)
	{
		float x = _mapLimits.Min.X - 51.0f;
		float y = _mapLimits.Min.Y - 51.0f;

		for (float xx = x; xx <= _mapLimits.Max.X + 51.0f; xx += 51.0f)
		{
			for (float yy = y; yy <= _mapLimits.Max.Y + 51.0f; yy += 51.0f)
			{
				_backSprite->SetPosition(glm::vec2(xx, yy));
				_backSprite->Draw(_jellyProjection);
			}
		}
	}
	else
	{
		float x = _worldLimits.Min.X - 51.0f;
		float y = _worldLimits.Min.Y - 51.0f;

		for (float xx = x; xx <= _worldLimits.Max.X + 51.0f; xx += 51.0f)
		{
			for (float yy = y; yy <= _worldLimits.Max.Y + 51.0f; yy += 51.0f)
			{
				_backSprite->SetPosition(glm::vec2(xx, yy));
				_backSprite->Draw(_jellyProjection);
			}
		}
	}	

	//camera aabb
	Vector2 camMin = _car->getPosition() - Vector2(20.0f, 11.2f);
	Vector2 camMax = _car->getPosition() + Vector2(20.0f, 11.2f);
	JellyPhysics::AABB camAABB(camMin, camMax);

	//ignore aabb
	Vector2 ignoreMin = _car->getPosition() - Vector2(40.0f, 22.4f);
	Vector2 ignoreMax = _car->getPosition() + Vector2(40.0f, 22.4f);
	JellyPhysics::AABB ignoreAABB(ignoreMin, ignoreMax);

	//render level bodies
	for (size_t i = 0; i < _gameBodies.size(); i++)
	{
		if (_showMap)
		{
			_gameBodies[i]->Draw(_jellyProjection);

			if (ignoreAABB.intersects(_gameBodies[i]->GetIgnoreAABB()))
			{
				_gameBodies[i]->SetIgnore(false);
			}
			else
			{
				_gameBodies[i]->SetIgnore(true);
			}
		}
		else
		{
			if (ignoreAABB.intersects(_gameBodies[i]->GetIgnoreAABB()))
			{
				_gameBodies[i]->SetIgnore(false);

				if (_gameBodies[i]->GetIgnoreAABB().intersects(camAABB))
				{
					_gameBodies[i]->Draw(_jellyProjection);
				}
			}
			else
			{
				_gameBodies[i]->SetIgnore(true);
			}
		}
	}

	//car
	_car->Draw(_jellyProjection);

	//timer
	char bufferTime[10];
	sprintf(bufferTime,"%.2f", _time);
	_menuFont->AddText(std::string(bufferTime) + "s", _renderManager->GetWidth() - 125, 50 + 2, glm::vec3(0.19f, 0.14f, 0.17f), FontLeft);
	_menuFont->AddText(std::string(bufferTime) + "s", _renderManager->GetWidth() - 125, 50, glm::vec3(0.0f, 0.84f, 0.0f), FontLeft);

	//jumping
	char bufferJump[10];
	sprintf(bufferJump, "%.2f", _bestJumpLenght);
	_menuFont->AddText(std::string(bufferJump) + "m", _renderManager->GetWidth() - 125, 80 + 2, glm::vec3(0.19f, 0.14f, 0.17f), FontLeft);
	_menuFont->AddText(std::string(bufferJump) + "m", _renderManager->GetWidth() - 125, 80, glm::vec3(0.71f, 0.16f, 0.18f), FontLeft);

	//sound debug
	//if ((_chassisHit > 3.0f))
	//{
	//	_menuFont->AddText("SOUND", _renderManager->GetWidth() - 525, 50 + 2, glm::vec3(0.19f, 0.14f, 0.17f), FontLeft);
	//}

	//ballon - tire
	for (size_t i = 0; i < _gameBodies.size(); i++)
	{
		if (!_gameBodies[i]->GetBody()->getDisable())
		{
			if (_gameBodies[i]->GetName() == "itemballoon")
			{
				//draw ballon
				_ballonSprite->SetScale(glm::vec2(0.02f, -0.02f));
				_ballonSprite->SetPosition(_gameBodies[i]->GetStartPosition());
				_ballonSprite->Draw(_jellyProjection);
			}
			else if (_gameBodies[i]->GetName() == "itemstick")
			{
				_tireSprite->SetScale(glm::vec2(0.02f, 0.02f));
				_tireSprite->SetPosition(_gameBodies[i]->GetStartPosition());
				_tireSprite->Draw(_jellyProjection);
			}
		}		
	}

	//end level target
	_targetSprite->Draw(_jellyProjection);

	DrawPowerUps();

	//_transformMeter
	if (mTransformMeter >= 0.0f)
	{
		float test = 1.0f - mTransformMeter;
		_transformMeter->SetPosition(glm::vec2(128 - (test * 64.0f),40.0f));
		_transformMeter->SetSize(glm::vec2(128 * mTransformMeter, 32));
		_transformMeter->Draw(_projection);
	}

	//the overlay text goes over the HUD
	_core->DrawText(_projection);

	_touch.Clear();
	if (_gamePlayState != GamePlayState::Play)
	{
		DrawOverlay();
		_core->DrawText(_projection);
	}

	//end render to texture
	/*_core->frameObject->UnBind();

	//render normal
	_renderManager->StartFrame();
	_renderManager->ClearScreen();

	_core->renderSprite->Draw(_projection);*/

	//end frame
	_renderManager->EndFrame();
}

