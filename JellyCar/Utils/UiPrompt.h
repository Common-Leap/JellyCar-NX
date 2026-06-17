#ifndef UiPrompt_H
#define UiPrompt_H

#include <Andromeda/Graphics/Text/TexturedFont.h>
#include "RectangleDrawer.h"
#include "UiLayout.h"
#include <string>

using namespace Andromeda::Graphics;

namespace UiPrompt
{
	int EstimateTextWidth(const char* text, float fontSize);

	void DrawInputBadge(RectangleDrawer* ui, TexturedFont* font, glm::mat4& projection,
		int cx, int cy, const char* badgeText, float badgeFontSize = UiLayout::SmallFontSize);

	void DrawPromptInSlot(RectangleDrawer* ui, TexturedFont* badgeFont, TexturedFont* labelFont,
		glm::mat4& projection, int slotX, int slotW, int barY, int barH,
		const char* badgeText, const char* labelText);

	void DrawPromptInRow(RectangleDrawer* ui, TexturedFont* badgeFont, TexturedFont* labelFont,
		glm::mat4& projection, int rowX, int rowW, int rowY, int rowH,
		const char* badgeText, const char* labelText);
}

#endif
