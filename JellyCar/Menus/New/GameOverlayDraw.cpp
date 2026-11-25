#include "../../Game/JellyGame.h"

#include "../../Utils/RectangleDrawer.h"
#include "../../Utils/UiTheme.h"
#include "../../Utils/UiLayout.h"
#include "../../Utils/UiPrompt.h"

#include <cmath>
#include <stdio.h>

void JellyGame::DrawOverlay()
{
	RectangleDrawer* ui = RectangleDrawer::Instance();

	int screenW = _renderManager->GetWidth();
	int screenH = _renderManager->GetHeight();

	const int rowH = 58;
	const int rowGap = 10;
	const int padH = 32;
	const int padV = 28;
	const int titleH = 64;
	const int statH = 70;

	bool paused = _gamePlayState == GamePlayState::Paused;
	bool finished = _gamePlayState == GamePlayState::Finish;

	int count = (int)_overlayItems.size();
	int w = 440;
	int h = titleH + padV + count * rowH + (count - 1) * rowGap + (finished ? statH + rowGap * 2 : 0);
	int x = (screenW - w) / 2;
	int y = (screenH - h) / 2;
	int cx = UiLayout::CenterX(x, w);

	int innerX = x + padH;
	int innerW = w - padH * 2;

	ui->DrawDimOverlay(screenW, screenH, 0.36f, _projection);
	ui->DrawPanel(x, y, w, h, UiTheme::PanelRadius(), _projection);

	_menuFont->AddText(OverlayTitle(), cx, UiLayout::BaselineCapCenter(y + 4, titleH, UiLayout::MenuFontSize),
		finished ? UiTheme::TextAccent() : UiTheme::TextHighlight(), FontCenter);

	int rowY = y + titleH;

	// result, green frame for a new record
	if (finished)
	{
		char bufTime[16], bufJump[16];
		snprintf(bufTime, sizeof(bufTime), "%.2fs", _time);
		snprintf(bufJump, sizeof(bufJump), "%.2fm", _bestJumpLenght);

		int statW = (innerW - rowGap) / 2;
		UiPrompt::DrawStat(ui, _core->_smallFont, _menuFont, UiLayout::MenuFontSize, _projection,
			innerX, rowY, statW, statH, _newTimeRecord ? "New record" : "Time", bufTime, UiTheme::TextSuccess(), _newTimeRecord);
		UiPrompt::DrawStat(ui, _core->_smallFont, _menuFont, UiLayout::MenuFontSize, _projection,
			innerX + statW + rowGap, rowY, statW, statH, _newJumpRecord ? "New record" : "Jump", bufJump, UiTheme::TextHighlight(), _newJumpRecord);

		rowY += statH + rowGap * 2;
	}

	float pulse = 0.5f + 0.5f * sinf(_overlayTime * 4.0f);

	for (int i = 0; i < count; i++)
	{
		bool selected = i == _overlaySelected;
		OverlayItem item = _overlayItems[i];

		ui->DrawCell(innerX, rowY, innerW, rowH, selected, pulse, _projection);
		_menuFont->AddText(OverlayLabel(item), cx, UiLayout::BaselineCapCenter(rowY, rowH, UiLayout::MenuFontSize),
			selected ? UiTheme::TextHighlight() : UiTheme::TextPrimary(), FontCenter);

		// buttons that do the same without selecting the entry
		MenuAction shortcut = MenuAction::MenuCount;
		if (item == OverlayRetry)
			shortcut = MenuAction::MenuRetry;
		else if (item == (paused ? OverlayResume : OverlayLevelSelect))
			shortcut = MenuAction::MenuBack;

		if (shortcut != MenuAction::MenuCount)
			UiPrompt::DrawButton(ui, _core->_badgeFont, _projection, innerX + innerW - 34, UiLayout::CenterY(rowY, rowH),
				_inputHelper->MenuActionButton(shortcut));

		_touch.Add(i, innerX, rowY - rowGap / 2, innerW, rowH + rowGap);
		rowY += rowH + rowGap;
	}
}
