#include "../../Game/JellyOptions.h"

#include "../../Utils/RectangleDrawer.h"
#include "../../Utils/UiTheme.h"
#include "../../Utils/UiLayout.h"
#include "../../Utils/UiPrompt.h"

#include <algorithm>
#include <stdio.h>

namespace
{
	const int PANEL_Y = 104;
	const int PANEL_H = 520;
	const int PANEL_PAD = 24;

	const int LIST_X = 80;
	const int LIST_W = 300;
	const int PAGE_X = LIST_X + LIST_W + 20;
	const int PAGE_W = 1280 - LIST_X - PAGE_X;

	//lines of the credits fade in and out over this height
	const float FADE_HEIGHT = 50.0f;
}

void JellyOptions::ArrangeMenu()
{
	_showBodies = false;

	_scrollTop = PANEL_Y + 34.0f;
	_scrollBottom = PANEL_Y + PANEL_H - 14.0f;
}

void JellyOptions::Draw(GameManager* manager)
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

	// --- title ---
	{
		int w = 380;
		int h = 76;
		int x = (screenW - w) / 2;
		int y = 12;
		ui->DrawPanel(x, y, w, h, UiTheme::BarRadius(), _projection);
		_titleFont->AddText("Options", UiLayout::CenterX(x, w),
			UiLayout::BaselineCapCenter(y, h, UiLayout::TitleFontSize) - 6, UiTheme::TextHighlight(), FontCenter);
	}

	bool inMenu = _optionsState == Menu;
	float pulse = std::max(0.0f, std::min(1.0f, _alphaScale));

	// --- list of pages ---
	{
		const int rowGap = 14;
		const int rowCount = (int)_menuItems.size();
		const int rowH = (PANEL_H - PANEL_PAD * 2 - rowGap * (rowCount - 1)) / rowCount;
		int x = LIST_X + 18;
		int w = LIST_W - 36;

		ui->DrawPanel(LIST_X, PANEL_Y, LIST_W, PANEL_H, UiTheme::PanelRadius(), _projection);

		for (size_t i = 0; i < _menuItems.size(); i++)
		{
			bool selected = (int)i == _menuSelected;
			int y = PANEL_Y + PANEL_PAD + (int)i * (rowH + rowGap);

			// the selected entry stands out less while its page has the focus
			if (selected && !inMenu)
				ui->DrawRoundedRect(x, y, w, rowH, UiTheme::CellRadius(), UiTheme::CellFillSelected(), UiTheme::CellBorder(), 1.0f, _projection);
			else
				ui->DrawCell(x, y, w, rowH, selected, pulse, _projection);

			_iconSprite->SetTexture(_menuItems[i].Image);
			_iconSprite->SetScale(glm::vec2(0.38f, 0.38f));
			_iconSprite->SetPosition(glm::vec2(x + 46, UiLayout::CenterY(y, rowH)));
			_iconSprite->Draw(_projection);

			_menuFont->AddText(_menuItems[i].Label, x + 88, UiLayout::BaselineCapCenter(y, rowH, UiLayout::MenuFontSize),
				!selected ? UiTheme::TextPrimary() : (inMenu ? UiTheme::TextHighlight() : UiTheme::TextAccent()), FontLeft);

			if (inMenu)
				_touch.Add(TouchItem + (int)i, x, y - rowGap / 2, w, rowH + rowGap);
		}
	}

	// --- page ---
	ui->DrawPanel(PAGE_X, PANEL_Y, PAGE_W, PANEL_H, UiTheme::PanelRadius(), _projection);

	int pageX = PAGE_X + PANEL_PAD;
	int pageW = PAGE_W - PANEL_PAD * 2;
	int pageY = PANEL_Y + PANEL_PAD;
	int pageH = PANEL_H - PANEL_PAD * 2;

	switch (CurrentPage())
	{
		case JellyOptionsState::Controls:
		{
			const int gap = 14;
			const int rows = (int)_carActions.size() / 2;
			int cellW = (pageW - gap) / 2;
			int cellH = (pageH - gap * (rows - 1)) / rows;

			for (int i = 0; i < (int)_carActions.size(); i++)
			{
				bool selected = !inMenu && i == _selctedPosition;
				bool binding = selected && _changeBinding;
				int x = pageX + (i / rows) * (cellW + gap);
				int y = pageY + (i % rows) * (cellH + gap);
				int cy = UiLayout::CenterY(y, cellH);

				ui->DrawCell(x, y, cellW, cellH, selected, pulse, _projection);

				if (binding)
				{
					glm::vec4 frame = UiTheme::CellBorderBinding();
					frame.a = 0.35f + 0.6f * pulse;
					ui->DrawRoundedRect(x, y, cellW, cellH, UiTheme::CellRadius(), UiTheme::CellFill(), frame, 3.0f, _projection);
				}

				_menuFont->AddText(_actionTranslation[_carActions[i]], x + 24, UiLayout::BaselineCapCenter(y, cellH, UiLayout::MenuFontSize),
					selected ? UiTheme::TextHighlight() : UiTheme::TextPrimary(), FontLeft);

				if (binding)
					_menuFont->AddText("...", x + cellW - 48, UiLayout::BaselineCapCenter(y, cellH, UiLayout::MenuFontSize), UiTheme::TextHighlight(), FontCenter);
				else
					UiPrompt::DrawButton(ui, badgeFont, _projection, x + cellW - 48, cy, _inputHelper->CarActionButton(_carActions[i]), 40);

				if (!inMenu)
					_touch.Add(TouchBinding + i, x, y, cellW, cellH);
			}
		}
		break;
		case JellyOptionsState::Sound:
		{
			const char* labels[] = { "Car", "Sounds", "Music" };
			const int gap = 14;
			const int barH = 26;
			int cellH = (pageH - gap * 2) / 3;

			_volumeBarX = pageX + 28;
			_volumeBarW = pageW - 56;

			for (int i = 0; i < 3; i++)
			{
				bool selected = !inMenu && _soundPosition == i;
				int y = pageY + i * (cellH + gap);
				int textY = UiLayout::BaselineCapCenter(y + 14, 48, UiLayout::MenuFontSize);
				glm::vec3 color = selected ? UiTheme::TextHighlight() : UiTheme::TextPrimary();

				ui->DrawCell(pageX, y, pageW, cellH, selected, pulse, _projection);
				_menuFont->AddText(labels[i], _volumeBarX, textY, color, FontLeft);

				char level[16];
				snprintf(level, sizeof(level), "%d%%", VolumeLevel(i) * 10);
				_menuFont->AddText(level, _volumeBarX + _volumeBarW, textY, color, FontRight);

				ui->DrawProgressBar(_volumeBarX, y + cellH - 30 - barH, _volumeBarW, barH, barH / 2.0f,
					VolumeLevel(i) / 10.0f, selected, _projection);

				if (!inMenu)
					_touch.Add(TouchVolume + i, pageX, y, pageW, cellH);
			}
		}
		break;
		case JellyOptionsState::Credits:
		case JellyOptionsState::Libs:
		{
			std::vector<Text>& lines = ScrollText();
			int cx = UiLayout::CenterX(PAGE_X, PAGE_W);

			for (size_t i = 0; i < lines.size(); i++)
			{
				float y = lines[i].StartPosition + _scrollPosition;
				if (y <= _scrollTop || y >= _scrollBottom)
					continue;

				// the panel is almost white, so fading is mixing with white
				float edge = std::min(y - _scrollTop, _scrollBottom - y);
				float visible = std::max(0.0f, std::min(1.0f, edge / FADE_HEIGHT));
				glm::vec3 color = i == 0 ? UiTheme::TextHighlight() : UiTheme::TextPrimary();

				(i == 0 ? _menuFont : bodyFont)->AddText(lines[i].Content, cx, (int)y,
					glm::mix(glm::vec3(0.97f, 0.97f, 0.98f), color, visible), FontCenter);
			}
		}
		break;
		default:
		break;
	}

	// --- bottom bar ---
	{
		std::vector<UiPrompt::Prompt> prompts;
		Gamepad::Button accept = _inputHelper->MenuActionButton(MenuAction::MenuAccept);
		Gamepad::Button back = _inputHelper->MenuActionButton(MenuAction::MenuBack);

		if (_optionsState == Controls && _changeBinding)
		{
			prompts.push_back({ _inputHelper->MenuActionButton(MenuAction::MenuExit), "Cancel", TouchBack });
		}
		else
		{
			if (inMenu)
				prompts.push_back({ accept, "Open", TouchItem + _menuSelected });

			if (_optionsState == Controls)
			{
				prompts.push_back({ accept, "Change", TouchBinding + _selctedPosition });
				prompts.push_back({ _inputHelper->MenuActionButton(MenuAction::MenuReset), "Reset", TouchReset });
			}

			prompts.push_back({ back, "Back", TouchBack });
		}

		UiPrompt::DrawBottomBar(ui, badgeFont, bodyFont, UiLayout::BodyFontSize, _projection, screenW, screenH, prompts, _touch);
	}

	_core->DrawText(_projection);

	_renderManager->EndFrame();
}
