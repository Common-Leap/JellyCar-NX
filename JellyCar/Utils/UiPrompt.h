#ifndef UiPrompt_H
#define UiPrompt_H

#include <Andromeda/Graphics/Text/TexturedFont.h>
#include "RectangleDrawer.h"
#include "UiLayout.h"
#include "InputHelper.h"
#include "TouchTargets.h"
#include <string>
#include <vector>

using namespace Andromeda::Graphics;

namespace UiPrompt
{
	const int ButtonSize = 30;

	struct Prompt
	{
		Gamepad::Button Button;
		std::string Label;
		int TouchId;
	};

	// Shorten text with dots when it is wider than maxWidth
	std::string FitText(TexturedFont* font, const std::string& text, int maxWidth);

	int ButtonWidth(Gamepad::Button button, int size = ButtonSize);

	// A drawn controller button, the letter uses the badge font
	void DrawButton(RectangleDrawer* ui, TexturedFont* badgeFont, glm::mat4& projection,
		int cx, int cy, Gamepad::Button button, int size = ButtonSize);

	int PromptWidth(TexturedFont* labelFont, Gamepad::Button button, const std::string& label);

	// Button and label, starting at x and centered on cy
	void DrawPrompt(RectangleDrawer* ui, TexturedFont* badgeFont, TexturedFont* labelFont, float labelSize,
		glm::mat4& projection, int x, int cy, Gamepad::Button button, const std::string& label);

	// Prompts in a panel at the bottom of the screen
	void DrawBottomBar(RectangleDrawer* ui, TexturedFont* badgeFont, TexturedFont* labelFont, float labelSize,
		glm::mat4& projection, int screenW, int screenH, const std::vector<Prompt>& prompts, TouchTargets& touch);

	// Name of a value above the value
	void DrawStat(RectangleDrawer* ui, TexturedFont* nameFont, TexturedFont* valueFont, float valueSize,
		glm::mat4& projection, int x, int y, int w, int h, const char* name, const char* value, glm::vec3 color, bool highlight);
}

#endif
