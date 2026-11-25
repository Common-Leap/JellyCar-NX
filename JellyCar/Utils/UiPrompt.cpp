#include "UiPrompt.h"
#include "UiTheme.h"

namespace UiPrompt
{
	namespace
	{
		const int LABEL_GAP = 10;
		const float BADGE_FONT_SIZE = 18.0f;

		const char* ButtonLetter(Gamepad::Button button)
		{
			switch (button)
			{
			case Gamepad::Button::Triangle:  return "X";
			case Gamepad::Button::Circle:    return "A";
			case Gamepad::Button::Cross:     return "B";
			case Gamepad::Button::Square:    return "Y";
			case Gamepad::Button::LTrigger:  return "L";
			case Gamepad::Button::RTrigger:  return "R";
			case Gamepad::Button::ZLTrigger: return "ZL";
			case Gamepad::Button::ZRTrigger: return "ZR";

			case Gamepad::Button::LAnalogLeft:
			case Gamepad::Button::LAnalogRight:
			case Gamepad::Button::LAnalogUp:
			case Gamepad::Button::LAnalogDown:
			case Gamepad::Button::LAnalogPushed: return "L";

			case Gamepad::Button::RAnalogLeft:
			case Gamepad::Button::RAnalogRight:
			case Gamepad::Button::RAnalogUp:
			case Gamepad::Button::RAnalogDown:
			case Gamepad::Button::RAnalogPushed: return "R";

			default: return "";
			}
		}

		bool IsShoulder(Gamepad::Button button)
		{
			return button == Gamepad::Button::LTrigger || button == Gamepad::Button::RTrigger
				|| button == Gamepad::Button::ZLTrigger || button == Gamepad::Button::ZRTrigger;
		}

		bool IsStick(Gamepad::Button button)
		{
			return button >= Gamepad::Button::LAnalogLeft && button <= Gamepad::Button::RAnalogPushed;
		}

		// Direction a d-pad button or a stick points in, 0 0 when it has none
		void Direction(Gamepad::Button button, int& x, int& y)
		{
			x = 0;
			y = 0;

			switch (button)
			{
			case Gamepad::Button::Left:
			case Gamepad::Button::LAnalogLeft:
			case Gamepad::Button::RAnalogLeft:
				x = -1;
				break;
			case Gamepad::Button::Right:
			case Gamepad::Button::LAnalogRight:
			case Gamepad::Button::RAnalogRight:
				x = 1;
				break;
			case Gamepad::Button::Up:
			case Gamepad::Button::LAnalogUp:
			case Gamepad::Button::RAnalogUp:
				y = -1;
				break;
			case Gamepad::Button::Down:
			case Gamepad::Button::LAnalogDown:
			case Gamepad::Button::RAnalogDown:
				y = 1;
				break;
			default:
				break;
			}
		}

		void Box(RectangleDrawer* ui, glm::mat4& projection, int cx, int cy, int w, int h, float radius, glm::vec4 color)
		{
			ui->DrawRoundedRect(cx - w / 2, cy - h / 2, w, h, radius, color, glm::vec4(0, 0, 0, 0), 0.0f, projection);
		}

		void Letter(TexturedFont* font, int cx, int cy, const char* text)
		{
			font->AddText(text, cx, cy + (int)(BADGE_FONT_SIZE * 0.36f), UiTheme::ButtonText(), FontCenter);
		}
	}

	std::string FitText(TexturedFont* font, const std::string& text, int maxWidth)
	{
		if (font->GetTextLenght(text) <= maxWidth)
			return text;

		std::string cut = text;
		while (!cut.empty() && font->GetTextLenght(cut + "...") > maxWidth)
			cut.erase(cut.size() - 1);

		return cut + "...";
	}

	int ButtonWidth(Gamepad::Button button, int size)
	{
		if (IsShoulder(button))
			return size * 3 / 2;

		if (IsStick(button))
			return size * 5 / 4;

		return size;
	}

	void DrawButton(RectangleDrawer* ui, TexturedFont* badgeFont, glm::mat4& projection,
		int cx, int cy, Gamepad::Button button, int size)
	{
		int w = ButtonWidth(button, size);
		int dirX, dirY;
		Direction(button, dirX, dirY);

		if (IsStick(button))
		{
			// ring with the stick pushed to one side
			int knob = size * 4 / 5;
			int reach = (w - knob) / 2 + 2;

			ui->DrawRoundedRect(cx - w / 2, cy - w / 2, w, w, w / 2.0f,
				UiTheme::ButtonRingFill(), UiTheme::ButtonFill(), 1.5f, projection);

			int kx = cx + dirX * reach;
			int ky = cy + dirY * reach;
			Box(ui, projection, kx, ky, knob, knob, knob / 2.0f, UiTheme::ButtonFill());
			Letter(badgeFont, kx, ky, ButtonLetter(button));
			return;
		}

		if (dirX != 0 || dirY != 0)
		{
			// d-pad with one arm lit
			int arm = size / 3;
			int lit = arm - 4;
			int offset = size / 2 - arm / 2 - 1;

			Box(ui, projection, cx, cy, size, arm, 3.0f, UiTheme::ButtonFill());
			Box(ui, projection, cx, cy, arm, size, 3.0f, UiTheme::ButtonFill());
			Box(ui, projection, cx + dirX * offset, cy + dirY * offset, lit, lit, 2.0f, UiTheme::ButtonLit());
			return;
		}

		float radius = IsShoulder(button) ? size * 0.36f : size / 2.0f;
		Box(ui, projection, cx, cy, w, size, radius, UiTheme::ButtonFill());

		if (button == Gamepad::Button::Select || button == Gamepad::Button::Start)
		{
			int bar = size / 2 - 2;
			Box(ui, projection, cx, cy, bar, 3, 1.0f, UiTheme::ButtonLit());
			if (button == Gamepad::Button::Start)
				Box(ui, projection, cx, cy, 3, bar, 1.0f, UiTheme::ButtonLit());
			return;
		}

		Letter(badgeFont, cx, cy, ButtonLetter(button));
	}

	int PromptWidth(TexturedFont* labelFont, Gamepad::Button button, const std::string& label)
	{
		return ButtonWidth(button) + LABEL_GAP + (int)labelFont->GetTextLenght(label);
	}

	void DrawPrompt(RectangleDrawer* ui, TexturedFont* badgeFont, TexturedFont* labelFont, float labelSize,
		glm::mat4& projection, int x, int cy, Gamepad::Button button, const std::string& label)
	{
		int w = ButtonWidth(button);
		DrawButton(ui, badgeFont, projection, x + w / 2, cy, button);
		labelFont->AddText(label, x + w + LABEL_GAP, cy + (int)(labelSize * 0.36f), UiTheme::TextPrimary(), FontLeft);
	}

	void DrawBottomBar(RectangleDrawer* ui, TexturedFont* badgeFont, TexturedFont* labelFont, float labelSize,
		glm::mat4& projection, int screenW, int screenH, const std::vector<Prompt>& prompts, TouchTargets& touch)
	{
		const int gap = 44;
		const int pad = 32;
		const int h = 48;
		const int y = screenH - h - 12;

		int w = pad * 2 + gap * ((int)prompts.size() - 1);
		for (size_t i = 0; i < prompts.size(); i++)
			w += PromptWidth(labelFont, prompts[i].Button, prompts[i].Label);

		int x = (screenW - w) / 2;
		ui->DrawPanel(x, y, w, h, UiTheme::BarRadius(), projection);

		x += pad;
		for (size_t i = 0; i < prompts.size(); i++)
		{
			int promptW = PromptWidth(labelFont, prompts[i].Button, prompts[i].Label);

			DrawPrompt(ui, badgeFont, labelFont, labelSize, projection, x, y + h / 2, prompts[i].Button, prompts[i].Label);
			touch.Add(prompts[i].TouchId, x - gap / 2, y - 10, promptW + gap, h + 22);

			x += promptW + gap;
		}
	}

	void DrawStat(RectangleDrawer* ui, TexturedFont* nameFont, TexturedFont* valueFont, float valueSize,
		glm::mat4& projection, int x, int y, int w, int h, const char* name, const char* value, glm::vec3 color, bool highlight)
	{
		ui->DrawRoundedRect(x, y, w, h, UiTheme::CellRadius(),
			highlight ? UiTheme::CellFillDone() : UiTheme::CellFill(),
			highlight ? UiTheme::PowerUpActive() : UiTheme::CellBorder(), highlight ? 2.0f : 1.0f, projection);

		int cx = x + w / 2;
		nameFont->AddText(name, cx, y + 19, UiTheme::TextMuted(), FontCenter);
		valueFont->AddText(value, cx, y + 19 + (h - 19) / 2 + (int)(valueSize * 0.36f), color, FontCenter);
	}
}
