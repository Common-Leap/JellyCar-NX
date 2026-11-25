#include "../../Game/JellyGame.h"

void JellyGame::DrawOverlay()
{
	const glm::vec3 shadow(0.19f, 0.14f, 0.17f);
	const glm::vec3 red(0.71f, 0.16f, 0.18f);
	const glm::vec3 orange(1.0f, 0.65f, 0.0f);
	const glm::vec3 green(0.0f, 0.84f, 0.0f);

	const int rowH = 44;

	bool paused = _gamePlayState == GamePlayState::Paused;

	int centerX = _renderManager->GetWidth() / 2;
	int records = (_newJumpRecord ? 1 : 0) + (_newTimeRecord ? 1 : 0);
	int rows = 1 + (_gamePlayState == GamePlayState::Finish ? records : 0) + (int)_overlayItems.size();
	int y = _renderManager->GetHeight() / 2 - rows * rowH / 2 + 10;

	_menuFont->AddText(OverlayTitle(), centerX, y + 2, shadow, FontCenter);
	_menuFont->AddText(OverlayTitle(), centerX, y, paused ? red : orange, FontCenter);
	y += rowH;

	if (_gamePlayState == GamePlayState::Finish && _newJumpRecord)
	{
		_menuFont->AddText("New Jump Record!", centerX, y + 2, shadow, FontCenter);
		_menuFont->AddText("New Jump Record!", centerX, y, red, FontCenter);
		y += rowH;
	}

	if (_gamePlayState == GamePlayState::Finish && _newTimeRecord)
	{
		_menuFont->AddText("New Time Record!", centerX, y + 2, shadow, FontCenter);
		_menuFont->AddText("New Time Record!", centerX, y, green, FontCenter);
		y += rowH;
	}

	for (size_t i = 0; i < _overlayItems.size(); i++)
	{
		bool selected = (int)i == _overlaySelected;
		std::string label = OverlayLabel(_overlayItems[i]);

		_menuFont->AddText(label, centerX, y, selected ? red : shadow, FontCenter);

		if (selected)
		{
			Sprite* button = _inputHelper->MenuActionSprite(MenuAction::MenuAccept);
			button->SetPosition(glm::vec2(centerX - (int)_menuFont->GetTextLenght(label) / 2 - 30, y - 10));
			button->Draw(_projection);
		}

		_touch.Add((int)i, centerX - 180, y - 32, 360, rowH);
		y += rowH;
	}
}
