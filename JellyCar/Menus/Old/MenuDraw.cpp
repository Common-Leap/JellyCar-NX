#include "../../Game/JellyMenuBetter.h"

#include <stdio.h>
#include <cmath>

void JellyMenuBetter::Draw(GameManager* manager)
{
	_touch.Clear();

	_renderManager->StartFrame();
	_renderManager->ClearScreen();

	// --- tiled paper background ---
	{
		int bw = _backSprite->GetTexture()->GetWidth();
		int bh = _backSprite->GetTexture()->GetHeight();
		int cols = (int)ceil((float)_renderManager->GetWidth()  / (float)bw);
		int rows = (int)ceil((float)_renderManager->GetHeight() / (float)bh);
		_backSprite->SetScale(glm::vec2(1.0f, 1.0f));
		for (int y = 0; y < rows; y++)
		{
			for (int x = 0; x < cols; x++)
			{
				_backSprite->SetPosition(glm::vec2(bw * x + bw / 2, bh * y + bh / 2));
				_backSprite->Draw(_projection);
			}
		}
	}

	// --- physics car preview (bottom strip) ---
	for (size_t i = 0; i < _gameBodies.size(); i++)
		_gameBodies[i]->Draw(_jellyProjection);
	_car->Draw(_jellyProjection);

	int screenW = _renderManager->GetWidth();
	int screenH = _renderManager->GetHeight();

	// --- main title at top ---
	_titleFont->AddText("JELLY CAR", screenW / 2, 66 + 2, glm::vec3(0.19f, 0.14f, 0.17f), FontCenter);
	_titleFont->AddText("JELLY CAR", screenW / 2, 66,     glm::vec3(0.71f, 0.16f, 0.18f), FontCenter);

	// --- selected level title (smaller, below main title) ---
	int selIdx = CurrentIndex();
	_menuFont->AddText(_sceneFiles[selIdx], screenW / 2, 112 + 2, glm::vec3(0.19f, 0.14f, 0.17f), FontCenter);
	_menuFont->AddText(_sceneFiles[selIdx], screenW / 2, 112,     glm::vec3(1.0f, 0.65f, 0.0f),   FontCenter);

	// --- layout: grid on left, image on right, centered ---
	const int CELL_W = 80;
	const int CELL_H = 65;
	const int GRID_WIDTH = GRID_COLS * CELL_W;  // 320px
	const int GRID_HEIGHT = GRID_ROWS * CELL_H; // 195px

	// thumbnail dimensions (scaled)
	float thumbScale = 0.95f;
	int thumbW = (int)(_levelImage->GetTexture()->GetWidth() * thumbScale);
	int thumbH = (int)(_levelImage->GetTexture()->GetHeight() * thumbScale);

	// total width of both panels + spacing
	int spacing = 50;
	int totalWidth = GRID_WIDTH + spacing + thumbW;
	int contentStartX = (screenW - totalWidth) / 2;

	// grid on left
	int gridStartX = contentStartX;
	int gridStartY = 180;

	int totalLevels = (int)_sceneFiles.size();
	int pageBase    = _currentPage * PAGE_SIZE;

	// draw grid cells
	for (int row = 0; row < GRID_ROWS; row++)
	{
		for (int col = 0; col < GRID_COLS; col++)
		{
			int idx = pageBase + row * GRID_COLS + col;
			if (idx >= totalLevels)
				continue;

			bool selected = (col == _gridCol && row == _gridRow);

			int cellX = gridStartX + col * CELL_W;
			int cellY = gridStartY + row * CELL_H;

			// draw rounded box behind cell (rounded_box.png is 70x55, drawn at 1:1 scale)
			_boxSprite->SetPosition(glm::vec2(cellX + CELL_W / 2, cellY + CELL_H / 2));
			_boxSprite->SetScale(glm::vec2(1.0f, 1.0f));
			_boxSprite->Draw(_projection);

			// level number top-left, time bottom-center
			char numBuf[8];
			sprintf(numBuf, "%d", idx + 1);
			glm::vec3 nameCol = selected ? glm::vec3(0.71f, 0.16f, 0.18f) : glm::vec3(0.19f, 0.14f, 0.17f);
			_menuFont->AddText(numBuf, cellX + 18, cellY + 30, nameCol, FontLeft);

			char timeBuf[12];
			float t = _levelManager->GetTime(_sceneFiles[idx]);
			if (t < 999.0f)
				sprintf(timeBuf, "%.1fs", t);
			else
				sprintf(timeBuf, "--");

			glm::vec3 timeCol = selected ? glm::vec3(0.0f, 0.84f, 0.0f) : glm::vec3(0.5f, 0.5f, 0.5f);
			_smallFont->AddText(timeBuf, cellX + CELL_W / 2, cellY + CELL_H - 16, timeCol, FontCenter);

			_touch.Add(TouchLevel + idx, cellX, cellY, CELL_W, CELL_H);

			// selection highlight - number already turns red via nameCol
		}
	}

	// page indicator below grid
	{
		char pageBuf[32];
		sprintf(pageBuf, "Page %d/%d", _currentPage + 1, _totalPages);
		int pageY = gridStartY + GRID_HEIGHT + 24;
		int pageX = gridStartX + GRID_WIDTH / 2;
		_smallFont->AddText(pageBuf, pageX, pageY, glm::vec3(0.71f, 0.16f, 0.18f), FontCenter);

		_inputHelper->MenuActionSprite(MenuAction::PagePrevious)->SetPosition(glm::vec2(pageX - 90, pageY - 5));
		_inputHelper->MenuActionSprite(MenuAction::PagePrevious)->Draw(_projection);
		_inputHelper->MenuActionSprite(MenuAction::PageNext)->SetPosition(glm::vec2(pageX + 90, pageY - 5));
		_inputHelper->MenuActionSprite(MenuAction::PageNext)->Draw(_projection);

		_touch.Add(TouchPagePrevious, gridStartX, pageY - 24, GRID_WIDTH / 2, 40);
		_touch.Add(TouchPageNext, pageX, pageY - 24, GRID_WIDTH / 2, 40);
	}

	// --- right panel: thumbnail + stats ---
	{
		int imageStartX = gridStartX + GRID_WIDTH + spacing;
		int imageStartY = gridStartY;

		// thumbnail (scaled)
		_levelImage->SetScale(glm::vec2(thumbScale, thumbScale));
		int thumbCX = imageStartX + thumbW / 2;
		int thumbCY = imageStartY + thumbH / 2;
		_levelImage->SetPosition(glm::vec2(thumbCX, thumbCY));
		_levelImage->Draw(_projection);
		_levelImage->SetScale(glm::vec2(1.0f, 1.0f)); // reset scale

		// time + jump stats below thumbnail (use menuFont for bigger text)
		int statsY = imageStartY + thumbH + 38;

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

		_menuFont->AddText(bufTime, thumbCX - 25, statsY + 1, glm::vec3(0.19f, 0.14f, 0.17f), FontRight);
		_menuFont->AddText(bufTime, thumbCX - 25, statsY,     glm::vec3(0.0f,  0.84f, 0.0f),  FontRight);

		_menuFont->AddText(bufJump, thumbCX + 25, statsY + 1, glm::vec3(0.19f, 0.14f, 0.17f), FontLeft);
		_menuFont->AddText(bufJump, thumbCX + 25, statsY,     glm::vec3(0.71f, 0.16f, 0.18f), FontLeft);

		// skin name (use smallFont)
		_smallFont->AddText(_carSkins[carcurrentPosition].name, thumbCX, statsY + 26 + 1, glm::vec3(0.19f, 0.14f, 0.17f), FontCenter);
		_smallFont->AddText(_carSkins[carcurrentPosition].name, thumbCX, statsY + 26,     glm::vec3(1.0f,  0.65f, 0.0f),  FontCenter);

		_inputHelper->MenuActionSprite(MenuAction::SkinPrevious)->SetPosition(glm::vec2(thumbCX - 120, statsY + 21));
		_inputHelper->MenuActionSprite(MenuAction::SkinPrevious)->Draw(_projection);
		_inputHelper->MenuActionSprite(MenuAction::SkinNext)->SetPosition(glm::vec2(thumbCX + 120, statsY + 21));
		_inputHelper->MenuActionSprite(MenuAction::SkinNext)->Draw(_projection);

		_touch.Add(TouchPlay, imageStartX, imageStartY, thumbW, thumbH);
		_touch.Add(TouchSkinPrevious, thumbCX - 160, statsY + 2, 160, 38);
		_touch.Add(TouchSkinNext, thumbCX, statsY + 2, 160, 38);
	}

	// --- bottom bar: options / exit ---
	{
		int barY = screenH - 29;
		int midX = screenW / 2;

		_inputHelper->MenuActionSprite(MenuAction::MenuPause)->SetPosition(glm::vec2(midX - 40, barY));
		_inputHelper->MenuActionSprite(MenuAction::MenuPause)->Draw(_projection);
		_menuFont->AddText("Options", midX - 90, screenH - 19, glm::vec3(0.19f, 0.14f, 0.17f), FontRight);

		_inputHelper->MenuActionSprite(MenuAction::MenuExit)->SetPosition(glm::vec2(midX + 40, barY));
		_inputHelper->MenuActionSprite(MenuAction::MenuExit)->Draw(_projection);
		_menuFont->AddText("Exit", midX + 90, screenH - 19, glm::vec3(0.19f, 0.14f, 0.17f), FontLeft);

		_touch.Add(TouchOptions, midX - 220, barY - 30, 210, 60);
		_touch.Add(TouchExit, midX + 10, barY - 30, 160, 60);
	}

	// flush text
	_core->DrawText(_projection);

	// --- exit question ---
	if (_confirmExit)
	{
		_touch.Clear();

		const glm::vec3 dark(0.19f, 0.14f, 0.17f);
		const glm::vec3 red(0.71f, 0.16f, 0.18f);

		int cx = screenW / 2;
		int cy = screenH / 2;

		// the box picture has rounded corners, big enough to cover the grid and the thumbnail
		_boxSprite->SetPosition(glm::vec2(cx, cy));
		_boxSprite->SetScale(glm::vec2(6.0f, 3.2f));
		_boxSprite->Draw(_projection);
		_boxSprite->SetScale(glm::vec2(1.0f, 1.0f));

		_menuFont->AddText("Exit game?", cx, cy - 18, dark, FontCenter);
		_menuFont->AddText("Exit", cx - 80, cy + 42, _confirmYes ? red : dark, FontCenter);
		_menuFont->AddText("Cancel", cx + 80, cy + 42, _confirmYes ? dark : red, FontCenter);

		_touch.Add(TouchExitYes, cx - 160, cy + 4, 150, 60);
		_touch.Add(TouchExitNo, cx + 10, cy + 4, 150, 60);

		_core->DrawText(_projection);
	}

	_renderManager->EndFrame();
}
