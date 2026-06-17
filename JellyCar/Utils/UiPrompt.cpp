#include "UiPrompt.h"
#include "UiTheme.h"
#include "UiLayout.h"
#include <cstring>

namespace UiPrompt
{
	int EstimateTextWidth(const char* text, float fontSize)
	{
		if (!text)
			return 0;

		float width = 0.0f;
		for (const char* p = text; *p; ++p)
		{
			char c = *p;
			if (c == ' ')
				width += fontSize * 0.28f;
			else if (c == 'i' || c == 'l' || c == 'I')
				width += fontSize * 0.30f;
			else if (c == 'W' || c == 'M')
				width += fontSize * 0.72f;
			else if (c == '-' || c == '+')
				width += fontSize * 0.40f;
			else
				width += fontSize * 0.52f;
		}

		return (int)width;
	}

	void DrawInputBadge(RectangleDrawer* ui, TexturedFont* font, glm::mat4& projection,
		int cx, int cy, const char* badgeText, float badgeFontSize)
	{
		int textW = EstimateTextWidth(badgeText, badgeFontSize);
		int badgeW = textW + 14;
		if (badgeW < 28)
			badgeW = 28;

		int badgeH = (int)(badgeFontSize * 1.55f);
		if (badgeH < 22)
			badgeH = 22;

		int badgeX = cx - badgeW / 2;
		int badgeY = cy - badgeH / 2;

		ui->DrawRoundedRect(badgeX, badgeY, badgeW, badgeH, badgeH * 0.35f,
			UiTheme::BadgeFill(), UiTheme::BadgeBorder(), 1.0f, projection);

		font->AddText(badgeText, cx,
			UiLayout::BaselineCenter(badgeY, badgeH, badgeFontSize),
			UiTheme::BadgeText(), FontCenter);
	}

	void DrawPromptInSlot(RectangleDrawer* ui, TexturedFont* badgeFont, TexturedFont* labelFont,
		glm::mat4& projection, int slotX, int slotW, int barY, int barH,
		const char* badgeText, const char* labelText)
	{
		const float badgeFontSize = UiLayout::SmallFontSize;
		const float labelFontSize = UiLayout::MenuFontSize;
		const int gap = 10;

		int badgeTextW = EstimateTextWidth(badgeText, badgeFontSize);
		int badgeW = badgeTextW + 14;
		if (badgeW < 28)
			badgeW = 28;

		int labelW = EstimateTextWidth(labelText, labelFontSize);
		int totalW = badgeW + gap + labelW;
		int slotCx = UiLayout::CenterX(slotX, slotW);
		int startX = slotCx - totalW / 2;
		int barCy = UiLayout::CenterY(barY, barH);
		int labelY = UiLayout::BaselineCenter(barY, barH, labelFontSize);

		DrawInputBadge(ui, badgeFont, projection, startX + badgeW / 2, barCy, badgeText, badgeFontSize);
		labelFont->AddText(labelText, startX + badgeW + gap, labelY, UiTheme::TextPrimary(), FontLeft);
	}

	void DrawPromptInRow(RectangleDrawer* ui, TexturedFont* badgeFont, TexturedFont* labelFont,
		glm::mat4& projection, int rowX, int rowW, int rowY, int rowH,
		const char* badgeText, const char* labelText)
	{
		DrawPromptInSlot(ui, badgeFont, labelFont, projection, rowX, rowW, rowY, rowH, badgeText, labelText);
	}
}
