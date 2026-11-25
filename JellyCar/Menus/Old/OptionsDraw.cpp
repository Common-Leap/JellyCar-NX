#include "../../Game/JellyOptions.h"

#include <algorithm>
#include <cmath>

void JellyOptions::ArrangeMenu()
{
	_showBodies = true;

	_scrollTop = _renderManager->GetWidth() * 0.13f;
	_scrollBottom = _renderManager->GetHeight() - (_renderManager->GetHeight() * 0.08f);

	//same order as on the screen
	std::sort(_menuItems.begin(), _menuItems.end(), [](const MenuItem& a, const MenuItem& b)
	{
		return a.Body->GetStartPosition().x < b.Body->GetStartPosition().x;
	});
}

void JellyOptions::Draw(GameManager* manager)
{
	_touch.Clear();

	//start frame
	_renderManager->StartFrame();

	//clear screen
	_renderManager->ClearScreen();

    //paper background
    {
        int backWidth = _backSprite->GetTexture()->GetWidth();
        int backHeight = _backSprite->GetTexture()->GetHeight();

        int columns = ceil((float)_renderManager->GetWidth() / (float)backWidth);
        int rows = ceil((float)_renderManager->GetHeight() / (float)backHeight);

        _backSprite->SetScale(glm::vec2(1.0f, 1.0f));

        for (size_t y = 0; y < rows; y++)
        {
            for (size_t x = 0; x < columns; x++)
            {
                int posx = (backWidth * x) + (backWidth / 2);
                int posy = (backHeight * y) + (backHeight / 2);

                _backSprite->SetPosition(glm::vec2(posx, posy));
                _backSprite->Draw(_projection);
            }
        }
    }

	//menu level drawing
	if (_optionsState == Menu)
	{
		for (size_t i = 0; i < _gameBodies.size(); i++)
		{
			_gameBodies[i]->SetLineColor(glm::vec4(0, 0, 0, 1.0f));
		}

		//set alpha value for selected menu
		_menuItems[_menuSelected].Body->SetLineColor(glm::vec4(0,0,1,_alphaScale));

		//menu level
		for (size_t i = 0; i < _gameBodies.size(); i++)
		{
			_gameBodies[i]->Draw(_jellyProjection);
		}
	}else
	{
		//menu level
		for (size_t i = 0; i < _gameBodies.size(); i++)
		{
			if (_gameBodies[i]->GetBody()->getIsStatic())
			{
				_gameBodies[i]->Draw(_jellyProjection);
			}			
		}
	}

	int centerX = _renderManager->GetWidth() / 2;

	switch (_optionsState)
	{
		case JellyOptionsState::Menu:
		{
			_titleFont->AddText("Options", centerX, 57, glm::vec3(0.19f, 0.14f, 0.17f), FontCenter);
			_titleFont->AddText("Options", centerX, 55, glm::vec3(1.0f, 0.65f, 0.0f), FontCenter);
		}
		break;
		case JellyOptionsState::Sound:
		{

			int leftSpritePosX = _renderManager->GetWidth() / 2 - (260);
			int rightSpritePosX = _renderManager->GetWidth() / 2 + (260);


			int leftRightPosY = _renderManager->GetHeight() / 2 + (_renderManager->GetHeight() * 0.08f);


			_titleFont->AddText("Sound Levels", centerX, 57, glm::vec3(0.19f, 0.14f, 0.17f), FontCenter);
			_titleFont->AddText("Sound Levels", centerX, 54, glm::vec3(1.0f, 0.65f, 0.0f), FontCenter);

			_menuFont->AddText("Car", leftSpritePosX + 30, leftRightPosY + 2 - 140 - 55, glm::vec3(0.19f, 0.14f, 0.17f), FontLeft);
			_menuFont->AddText("Car", leftSpritePosX + 30, leftRightPosY - 140 - 55, glm::vec3(1.0f, 0.65f, 0.0f), FontLeft);

			_menuFont->AddText("Sounds", leftSpritePosX + 30, leftRightPosY + 2 - 55, glm::vec3(0.19f, 0.14f, 0.17f), FontLeft);
			_menuFont->AddText("Sounds", leftSpritePosX + 30, leftRightPosY - 55, glm::vec3(1.0f, 0.65f, 0.0f), FontLeft);

			_menuFont->AddText("Music", leftSpritePosX + 30, leftRightPosY + 2 + 140 - 55, glm::vec3(0.19f, 0.14f, 0.17f), FontLeft);
			_menuFont->AddText("Music", leftSpritePosX + 30, leftRightPosY + 140 - 55, glm::vec3(1.0f, 0.65f, 0.0f), FontLeft);

			//rounded back
			//_backRoundSprite->SetSolor(glm::vec4(1.0f, 1.0f, 1.0f, _alphaScale));
			//_backRoundSprite->SetScale(glm::vec2(0.8f, 0.8f));

			//_backRoundSprite->SetPosition(glm::vec2(210, 170 + (_soundPosition * 140)));
			//_backRoundSprite->Draw(_projection);

			//_backRoundSprite->SetPosition(glm::vec2(750, 170 + (_soundPosition * 140)));
			//_backRoundSprite->Draw(_projection);

			//left side
			_leftSprite->SetScale(glm::vec2(0.8f, 0.8f));


            if (_soundPosition == 0)
            {
				_leftSprite->SetSolor(glm::vec4(1.0f, 1.0f, 1.0f, _alphaScale));
            }else
            {
				_leftSprite->SetSolor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
            }
			_leftSprite->SetPosition(glm::vec2(leftSpritePosX, leftRightPosY - 140));
			_leftSprite->Draw(_projection);

			if (_soundPosition == 1)
			{
				_leftSprite->SetSolor(glm::vec4(1.0f, 1.0f, 1.0f, _alphaScale));
			}
			else
			{
				_leftSprite->SetSolor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
			}
			_leftSprite->SetPosition(glm::vec2(leftSpritePosX, leftRightPosY));
			_leftSprite->Draw(_projection);


			if (_soundPosition == 2)
			{
				_leftSprite->SetSolor(glm::vec4(1.0f, 1.0f, 1.0f, _alphaScale));
			}
			else
			{
				_leftSprite->SetSolor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
			}
			_leftSprite->SetPosition(glm::vec2(leftSpritePosX, leftRightPosY + 140));
			_leftSprite->Draw(_projection);

			//right side
			_rightSprite->SetScale(glm::vec2(0.8f, 0.8f));

			if (_soundPosition == 0)
			{
				_rightSprite->SetSolor(glm::vec4(1.0f, 1.0f, 1.0f, _alphaScale));
			}
			else
			{
				_rightSprite->SetSolor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
			}
			_rightSprite->SetPosition(glm::vec2(rightSpritePosX, leftRightPosY - 140));
			_rightSprite->Draw(_projection);


			if (_soundPosition == 1)
			{
				_rightSprite->SetSolor(glm::vec4(1.0f, 1.0f, 1.0f, _alphaScale));
			}
			else
			{
				_rightSprite->SetSolor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
			}
			_rightSprite->SetPosition(glm::vec2(rightSpritePosX, leftRightPosY));
			_rightSprite->Draw(_projection);


			if (_soundPosition == 2)
			{
				_rightSprite->SetSolor(glm::vec4(1.0f, 1.0f, 1.0f, _alphaScale));
			}
			else
			{
				_rightSprite->SetSolor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
			}
			_rightSprite->SetPosition(glm::vec2(rightSpritePosX, leftRightPosY + 140));
			_rightSprite->Draw(_projection);

			//sound bars
			_barSprite->SetScale(glm::vec2(0.8f, 0.8f));
			_barBlueSprite->SetScale(glm::vec2(0.8f, 0.8f));


			int startBar = _renderManager->GetWidth() / 2 - (180);

			//int startBar = 300;

			//touch, the bars are 40 apart
			_volumeBarX = startBar - 20;
			_volumeBarW = 400;

			for (int i = 0; i < 3; i++)
			{
				_touch.Add(TouchVolume + i, _volumeBarX - 40, leftRightPosY + (i - 1) * 140 - 40, _volumeBarW + 80, 80);
			}

			//car
			for (int i = 0; i < 10;i++)
			{
				if (i < _optionsCarLevel)
				{
					_barSprite->SetPosition(glm::vec2(startBar + (i * 40), leftRightPosY - 140));
					_barSprite->Draw(_projection);
				}else
				{
					_barBlueSprite->SetPosition(glm::vec2(startBar + (i * 40), leftRightPosY - 140));
					_barBlueSprite->Draw(_projection);
				}
			}

			//sounds
			for (int i = 0; i < 10; i++)
			{
				if (i < _optionsSoundLevel)
				{
					_barSprite->SetPosition(glm::vec2(startBar + (i * 40), leftRightPosY ));
					_barSprite->Draw(_projection);
				}else
				{
					_barBlueSprite->SetPosition(glm::vec2(startBar + (i * 40), leftRightPosY ));
					_barBlueSprite->Draw(_projection);
				}
			}

			//music
			for (int i = 0; i < 10; i++)
			{
				if (i < _optionsMusicLevel)
				{
					_barSprite->SetPosition(glm::vec2(startBar + (i * 40), leftRightPosY + 140));
					_barSprite->Draw(_projection);
				}else
				{
					_barBlueSprite->SetPosition(glm::vec2(startBar + (i * 40), leftRightPosY + 140));
					_barBlueSprite->Draw(_projection);					
				}
			}
		}
		break;
		case JellyOptionsState::Controls: 
		{
			if (_changeBinding)
			{
				_backSelectSprite->SetSolor(glm::vec4(1.0f, 1.0f, 1.0f, _alphaScale));
			}
			else
			{
				_backSelectSprite->SetSolor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
			}

			int startPosition = _renderManager->GetHeight() / 2 - 100;
			int step = 45;

			//draw backgroud bar
			_backSelectSprite->SetScale(glm::vec2(0.5f,1.0f));

			if (_selctedPosition < 4)
			{
				_backSelectSprite->SetPosition(glm::vec2(centerX - 128, startPosition - 10 + (_selctedPosition * step)));
			}
			else
			{
				_backSelectSprite->SetPosition(glm::vec2(centerX + 128, startPosition - 10 + (_selctedPosition * step) - (step * 4)));
			}			
			
			_backSelectSprite->Draw(_projection);

			_titleFont->AddText("Car Controls", centerX, 57, glm::vec3(0.19f, 0.14f, 0.17f), FontCenter);
			_titleFont->AddText("Car Controls", centerX, 54, glm::vec3(1.0f, 0.65f, 0.0f), FontCenter);			

			//touch
			for (size_t i = 0; i < _carActions.size(); i++)
			{
				_touch.Add(TouchBinding + i, i < 4 ? centerX - 256 : centerX, startPosition - 10 - step / 2 + ((int)i % 4) * step, 256, step);
			}

			//reset
			{
				int resetY = startPosition + step * 5;
				Sprite* resetSprite = _inputHelper->MenuActionSprite(MenuAction::MenuReset);
				resetSprite->SetPosition(glm::vec2(centerX - 40, resetY - 10));
				resetSprite->Draw(_projection);

				_menuFont->AddText("Reset", centerX - 10, resetY, glm::vec3(0.19f, 0.14f, 0.17f), FontLeft);
				_touch.Add(TouchReset, centerX - 70, resetY - 32, 180, 44);
			}

			//draw text
			int startY = startPosition;

			//7 left positions
			for (size_t i = 0; i < 4; i++)
			{
				_menuFont->AddText(_actionTranslation[_carActions[i]], centerX - 90, startY, glm::vec3(0.19f, 0.14f, 0.17f), FontRight);
				_menuFont->AddText(_actionTranslation[_carActions[i]], centerX - 90, startY - 2, glm::vec3(1.0f, 0.65f, 0.0f), FontRight);

				startY += step;
			}

			//7 right positions
			startY = startPosition;
			for (size_t i = 4; i < _carActions.size(); i++)
			{
				_menuFont->AddText(_actionTranslation[_carActions[i]], centerX + 90, startY, glm::vec3(0.19f, 0.14f, 0.17f), FontLeft);
				_menuFont->AddText(_actionTranslation[_carActions[i]], centerX + 90, startY - 2 , glm::vec3(1.0f, 0.65f, 0.0f), FontLeft);

				startY += step;
			}

			//draw 7 left icons
			startY = startPosition - 10;
			for (size_t i = 0; i < 4; i++)
			{
				//InputAction action = static_cast<InputAction>(i);

				if (!(_changeBinding && i == _selctedPosition))
				{
					_inputHelper->CarActionSprite(_carActions[i])->SetPosition(glm::vec2(centerX - 40, startY));
					_inputHelper->CarActionSprite(_carActions[i])->Draw(_projection);

					//_menuFont->AddText(_inputHelper->InputName(action), glm::vec2(centerX - 40, startY + 10), glm::vec3(0.19f, 0.14f, 0.17f), FontCenter);
				}

				startY += step;
			}

			//draw 7 right icons
			startY = startPosition - 10;
			for (size_t i = 4; i < _carActions.size(); i++)
			{
				//InputAction action = static_cast<InputAction>(i);

				if (!(_changeBinding && i == _selctedPosition))
				{
					_inputHelper->CarActionSprite(_carActions[i])->SetPosition(glm::vec2(centerX + 40, startY));
					_inputHelper->CarActionSprite(_carActions[i])->Draw(_projection);

					//_menuFont->AddText(_inputHelper->InputName(action), glm::vec2(centerX + 40, startY + 10), glm::vec3(0.19f, 0.14f, 0.17f), FontCenter);
				}				

				startY += step;
			}
		}
		break;
		case JellyOptionsState::Credits:
		{
			_titleFont->AddText("Credits", centerX, 57, glm::vec3(0.19f, 0.14f, 0.17f), FontCenter);
			_titleFont->AddText("Credits", centerX, 54, glm::vec3(1.0f, 0.65f, 0.0f), FontCenter);

			for (size_t i = 0; i < _credits.size(); i++)
			{
				if (_credits[i].StartPosition + _scrollPosition  > _scrollTop && _credits[i].StartPosition + _scrollPosition < _scrollBottom)
				{
					_menuFont->AddText(_credits[i].Content, centerX, _credits[i].StartPosition + _scrollPosition, glm::vec3(0.19f, 0.14f, 0.17f), FontCenter);
					_menuFont->AddText(_credits[i].Content, centerX, _credits[i].StartPosition - 2 + _scrollPosition, glm::vec3(1.0f, 0.65f, 0.0f), FontCenter);
				}
			}

		}
		break;
		case JellyOptionsState::Libs:
		{
			_titleFont->AddText("Libs", _renderManager->GetWidth() / 2, 57, glm::vec3(0.19f, 0.14f, 0.17f), FontCenter);
			_titleFont->AddText("Libs", _renderManager->GetWidth() / 2, 54, glm::vec3(1.0f, 0.65f, 0.0f), FontCenter);

			for (size_t i = 0; i < _libs.size(); i++)
			{
				if (_libs[i].StartPosition + _scrollPosition  > _scrollTop && _libs[i].StartPosition + _scrollPosition < _scrollBottom)
				{
					_menuFont->AddText(_libs[i].Content, centerX, _libs[i].StartPosition + _scrollPosition, glm::vec3(0.19f, 0.14f, 0.17f), FontCenter);
					_menuFont->AddText(_libs[i].Content, centerX, _libs[i].StartPosition - 2 + _scrollPosition, glm::vec3(1.0f, 0.65f, 0.0f), FontCenter);
				}
			}
		}
		break;
		case JellyOptionsState::Secret:
		{
			//?
		}
		break;
	}

	//back to menu
	//_inputHelper->ActionSprite(InputAction::Back)->SetPosition(glm::vec2((_renderManager->GetWidth() / 2), 528));
	_inputHelper->MenuActionSprite(MenuAction::MenuBack)->SetPosition(glm::vec2((_renderManager->GetWidth() / 2), _renderManager->GetHeight() - 20));
	_inputHelper->MenuActionSprite(MenuAction::MenuBack)->Draw(_projection);

	//back text
	//_menuFont->AddText("Back", (_renderManager->GetWidth() / 2) + 30, 538, glm::vec3(0.19f, 0.14f, 0.17f), FontLeft);
	_menuFont->AddText("Back", (_renderManager->GetWidth() / 2) + 30, _renderManager->GetHeight() - 6 , glm::vec3(0.19f, 0.14f, 0.17f), FontLeft);

	_touch.Add(TouchBack, _renderManager->GetWidth() / 2 - 60, _renderManager->GetHeight() - 48, 220, 48);

	//draw main text
	_menuFont->Draw(_projection);
	_titleFont->Draw(_projection);
	_smallFont->Draw(_projection);

	//end frame
	_renderManager->EndFrame();
}
