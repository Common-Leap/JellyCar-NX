#include "../../Game/JellyMenuBetter.h"

#include "../../Utils/RectangleDrawer.h"
#include "../../Utils/UiTheme.h"
#include "../../Utils/UiLayout.h"
#include "../../Utils/UiPrompt.h"

#include <cmath>
#include <stdio.h>
#include <string>

namespace
{
	const int CELL_W = 100;
	const int CELL_H = 80;
	const int CELL_GAP = 10;
	const int GRID_PAD = 18;

	const int PANEL_Y = 98;
	const int PANEL_H = 336;
	const int PANEL_GAP = 20;
	const int DETAIL_W = 356;

	//time for the selection box to get most of the way to another cell
	const float CURSOR_TIME = 0.05f;

	//more pages than this are shown as numbers
	const int MAX_DOTS = 14;
}

void JellyMenuBetter::Draw(GameManager* manager)
{
	RectangleDrawer* ui = RectangleDrawer::Instance();
	TexturedFont* bodyFont = _core->_bodyFont;
	TexturedFont* badgeFont = _core->_badgeFont;

	_touch.Clear();

	_renderManager->StartFrame();
	_renderManager->ClearScreen();

	int screenW = _renderManager->GetWidth();
	int screenH = _renderManager->GetHeight();

	ui->DrawPaperBackground(_backSprite, screenW, screenH, _projection);

	// --- car to drive around with, at the bottom ---
	for (size_t i = 0; i < _gameBodies.size(); i++)
		_gameBodies[i]->Draw(_jellyProjection);
	_car->Draw(_jellyProjection);

	// --- title ---
	{
		int w = 380;
		int h = 68;
		int x = (screenW - w) / 2;
		int y = 16;
		ui->DrawPanel(x, y, w, h, UiTheme::BarRadius(), _projection);
		_titleFont->AddText("JELLY CAR", UiLayout::CenterX(x, w),
			UiLayout::BaselineCapCenter(y, h, UiLayout::TitleFontSize),
			UiTheme::TextHighlight(), FontCenter);
	}

	const int gridW = GRID_COLS * CELL_W + (GRID_COLS - 1) * CELL_GAP;
	const int gridH = GRID_ROWS * CELL_H + (GRID_ROWS - 1) * CELL_GAP;
	const int gridPanelW = gridW + GRID_PAD * 2;

	int gridPanelX = (screenW - (gridPanelW + PANEL_GAP + DETAIL_W)) / 2;
	int detailX = gridPanelX + gridPanelW + PANEL_GAP;
	int gridX = gridPanelX + GRID_PAD;
	int gridY = PANEL_Y + GRID_PAD;

	// --- level grid ---
	ui->DrawPanel(gridPanelX, PANEL_Y, gridPanelW, PANEL_H, UiTheme::PanelRadius(), _projection);

	int totalLevels = (int)_sceneFiles.size();
	int pageBase = _currentPage * PAGE_SIZE;

	for (int cell = 0; cell < PAGE_SIZE && pageBase + cell < totalLevels; cell++)
	{
		int idx = pageBase + cell;
		int x = gridX + (cell % GRID_COLS) * (CELL_W + CELL_GAP);
		int y = gridY + (cell / GRID_COLS) * (CELL_H + CELL_GAP);
		int cx = UiLayout::CenterX(x, CELL_W);

		bool selected = idx == CurrentIndex();
		float time = _levelManager->GetTime(_sceneFiles[idx]);
		bool done = time < 999.0f;

		ui->DrawRoundedRect(x, y, CELL_W, CELL_H, UiTheme::CellRadius(),
			done ? UiTheme::CellFillDone() : UiTheme::CellFill(), UiTheme::CellBorder(), 1.0f, _projection);

		char number[16];
		snprintf(number, sizeof(number), "%d", idx + 1);
		_menuFont->AddText(number, cx, done ? y + 39 : UiLayout::BaselineCapCenter(y, CELL_H, UiLayout::MenuFontSize),
			selected ? UiTheme::TextHighlight() : UiTheme::TextPrimary(), FontCenter);

		if (done)
		{
			char text[16];
			snprintf(text, sizeof(text), "%.1fs", time);
			bodyFont->AddText(text, cx, y + 67, UiTheme::TextSuccess(), FontCenter);
		}

		_touch.Add(TouchLevel + idx, x - CELL_GAP / 2, y - CELL_GAP / 2, CELL_W + CELL_GAP, CELL_H + CELL_GAP);
	}

	// selection box
	{
		float targetX = (float)(gridX + _gridCol * (CELL_W + CELL_GAP));
		float targetY = (float)(gridY + _gridRow * (CELL_H + CELL_GAP));

		float step = _cursorPlaced ? 1.0f - expf(-_dt / CURSOR_TIME) : 1.0f;
		_cursorX += (targetX - _cursorX) * step;
		_cursorY += (targetY - _cursorY) * step;
		_cursorPlaced = true;

		ui->DrawCell((int)(_cursorX + 0.5f), (int)(_cursorY + 0.5f), CELL_W, CELL_H, true, _focusPulse, _projection);
	}

	// pages, between the buttons that change them
	{
		int y = gridY + gridH;
		int h = PANEL_Y + PANEL_H - y;
		int cx = UiLayout::CenterX(gridPanelX, gridPanelW);
		int cy = UiLayout::CenterY(y, h);
		int buttonOffset = gridW / 2 - UiPrompt::ButtonWidth(Gamepad::Button::LTrigger) / 2;

		UiPrompt::DrawButton(ui, badgeFont, _projection, cx - buttonOffset, cy, _inputHelper->MenuActionButton(MenuAction::PagePrevious));
		UiPrompt::DrawButton(ui, badgeFont, _projection, cx + buttonOffset, cy, _inputHelper->MenuActionButton(MenuAction::PageNext));

		_touch.Add(TouchPagePrevious, gridPanelX, y, gridPanelW / 2, h);
		_touch.Add(TouchPageNext, cx, y, gridPanelW / 2, h);

		if (_totalPages <= MAX_DOTS)
		{
			const int dot = 10;
			const int active = 26;
			const int gap = 9;

			int x = cx - ((_totalPages - 1) * (dot + gap) + active) / 2;
			for (int i = 0; i < _totalPages; i++)
			{
				int w = i == _currentPage ? active : dot;
				ui->DrawRoundedRect(x, cy - dot / 2, w, dot, dot / 2.0f,
					i == _currentPage ? UiTheme::CellBorderSelected() : UiTheme::PanelBorder(), glm::vec4(0, 0, 0, 0), 0.0f, _projection);
				x += w + gap;
			}
		}
		else
		{
			char page[32];
			snprintf(page, sizeof(page), "%d / %d", _currentPage + 1, _totalPages);
			bodyFont->AddText(page, cx, UiLayout::BaselineCapCenter(y, h, UiLayout::BodyFontSize), UiTheme::TextMuted(), FontCenter);
		}
	}

	// --- selected level: name, picture, records, car skin ---
	{
		const std::string& scene = _sceneFiles[CurrentIndex()];
		int cx = UiLayout::CenterX(detailX, DETAIL_W);
		int innerX = detailX + GRID_PAD;
		int innerW = DETAIL_W - GRID_PAD * 2;

		ui->DrawPanel(detailX, PANEL_Y, DETAIL_W, PANEL_H, UiTheme::PanelRadius(), _projection);

		// name, in a smaller font when it is too long
		bool small = _menuFont->GetTextLenght(scene) > innerW;
		TexturedFont* nameFont = small ? bodyFont : _menuFont;
		float nameSize = small ? UiLayout::BodyFontSize : UiLayout::MenuFontSize;
		nameFont->AddText(UiPrompt::FitText(nameFont, scene, innerW), cx,
			UiLayout::BaselineCapCenter(PANEL_Y + 8, 42, nameSize), UiTheme::TextAccent(), FontCenter);

		const int imageY = PANEL_Y + 54;
		const int imageH = 160;

		float scale = (float)imageH / _levelImage->GetTexture()->GetHeight();
		if (_levelImage->GetTexture()->GetWidth() * scale > innerW)
			scale = (float)innerW / _levelImage->GetTexture()->GetWidth();

		int frameW = (int)(_levelImage->GetTexture()->GetWidth() * scale) + 8;
		int frameH = (int)(_levelImage->GetTexture()->GetHeight() * scale) + 8;
		ui->DrawRoundedRect(cx - frameW / 2, imageY + imageH / 2 - frameH / 2, frameW, frameH, 8.0f,
			glm::vec4(1.0f, 1.0f, 1.0f, 0.95f), UiTheme::CellBorder(), 1.0f, _projection);

		_levelImage->SetScale(glm::vec2(scale, scale));
		_levelImage->SetPosition(glm::vec2(cx, imageY + imageH / 2));
		_levelImage->Draw(_projection);
		_levelImage->SetScale(glm::vec2(1.0f, 1.0f));
		_touch.Add(TouchPlay, innerX, imageY, innerW, imageH);

		// records
		char bufTime[16], bufJump[16];
		float recTime = _levelManager->GetTime(scene);
		float recJump = _levelManager->GetJump(scene);
		snprintf(bufTime, sizeof(bufTime), recTime < 999.0f ? "%.2fs" : "--", recTime);
		snprintf(bufJump, sizeof(bufJump), recJump > 0.0f ? "%.2fm" : "--", recJump);

		const int statY = imageY + imageH + 10;
		const int statH = 60;
		int statW = (innerW - CELL_GAP) / 2;
		UiPrompt::DrawStat(ui, _smallFont, _menuFont, UiLayout::MenuFontSize, _projection,
			innerX, statY, statW, statH, "Time", bufTime, UiTheme::TextSuccess(), false);
		UiPrompt::DrawStat(ui, _smallFont, _menuFont, UiLayout::MenuFontSize, _projection,
			innerX + statW + CELL_GAP, statY, statW, statH, "Jump", bufJump, UiTheme::TextHighlight(), false);

		// car skin, between the buttons that change it
		const int skinY = statY + statH;
		int skinH = PANEL_Y + PANEL_H - skinY;
		int skinCy = UiLayout::CenterY(skinY, skinH);
		int buttonW = UiPrompt::ButtonWidth(Gamepad::Button::ZLTrigger);
		int buttonOffset = innerW / 2 - buttonW / 2;

		UiPrompt::DrawButton(ui, badgeFont, _projection, cx - buttonOffset, skinCy, _inputHelper->MenuActionButton(MenuAction::SkinPrevious));
		UiPrompt::DrawButton(ui, badgeFont, _projection, cx + buttonOffset, skinCy, _inputHelper->MenuActionButton(MenuAction::SkinNext));
		bodyFont->AddText(UiPrompt::FitText(bodyFont, _carSkins[carcurrentPosition].name, innerW - buttonW * 2 - 20), cx,
			UiLayout::BaselineCapCenter(skinY, skinH, UiLayout::BodyFontSize), UiTheme::TextAccent(), FontCenter);

		_touch.Add(TouchSkinPrevious, detailX, skinY, DETAIL_W / 2, skinH);
		_touch.Add(TouchSkinNext, cx, skinY, DETAIL_W / 2, skinH);
	}

	// --- bottom bar ---
	{
		std::vector<UiPrompt::Prompt> prompts;
		prompts.push_back({ _inputHelper->MenuActionButton(MenuAction::MenuAccept), "Play", TouchPlay });
		prompts.push_back({ _inputHelper->MenuActionButton(MenuAction::MenuPause), "Options", TouchOptions });
		prompts.push_back({ _inputHelper->MenuActionButton(MenuAction::MenuExit), "Exit", TouchExit });

		UiPrompt::DrawBottomBar(ui, badgeFont, bodyFont, UiLayout::BodyFontSize, _projection, screenW, screenH, prompts, _touch);
	}

	_core->DrawText(_projection);

	// --- exit question ---
	if (_confirmExit)
	{
		_touch.Clear();

		const int w = 440;
		const int h = 190;
		const int pad = 28;
		const int buttonH = 60;
		const int gap = 16;
		int x = (screenW - w) / 2;
		int y = (screenH - h) / 2;
		int buttonW = (w - pad * 2 - gap) / 2;
		int buttonY = y + h - pad - buttonH;

		ui->DrawDimOverlay(screenW, screenH, 0.40f, _projection);
		ui->DrawPanel(x, y, w, h, UiTheme::PanelRadius(), _projection);
		_menuFont->AddText("Exit game?", UiLayout::CenterX(x, w),
			UiLayout::BaselineCapCenter(y + 18, 66, UiLayout::MenuFontSize), UiTheme::TextAccent(), FontCenter);

		for (int i = 0; i < 2; i++)
		{
			bool selected = _confirmYes == (i == 0);
			int bx = x + pad + i * (buttonW + gap);

			ui->DrawCell(bx, buttonY, buttonW, buttonH, selected, _focusPulse, _projection);
			_menuFont->AddText(i == 0 ? "Exit" : "Cancel", UiLayout::CenterX(bx, buttonW),
				UiLayout::BaselineCapCenter(buttonY, buttonH, UiLayout::MenuFontSize),
				selected ? UiTheme::TextHighlight() : UiTheme::TextPrimary(), FontCenter);
			_touch.Add(i == 0 ? TouchExitYes : TouchExitNo, bx, buttonY, buttonW, buttonH);
		}

		_core->DrawText(_projection);
	}

	_renderManager->EndFrame();
}
