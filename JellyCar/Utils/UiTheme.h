#ifndef UiTheme_H
#define UiTheme_H

#include <glm/glm.hpp>

namespace UiTheme
{
	// Neutral frosted glass on paper
	inline glm::vec4 PanelFill()    { return glm::vec4(0.98f, 0.99f, 1.00f, 0.82f); }
	inline glm::vec4 PanelBorder()  { return glm::vec4(0.20f, 0.22f, 0.28f, 0.18f); }
	inline glm::vec4 PanelShadow()  { return glm::vec4(0.00f, 0.00f, 0.00f, 0.08f); }

	inline glm::vec4 CellFill()          { return glm::vec4(1.00f, 1.00f, 1.00f, 0.74f); }
	inline glm::vec4 CellFillSelected()  { return glm::vec4(0.93f, 0.96f, 1.00f, 0.84f); }
	inline glm::vec4 CellBorder()        { return glm::vec4(0.20f, 0.22f, 0.28f, 0.14f); }
	inline glm::vec4 CellBorderSelected(){ return glm::vec4(0.38f, 0.50f, 0.78f, 0.65f); }

	inline glm::vec4 HighlightFill() { return glm::vec4(0.93f, 0.96f, 1.00f, 0.76f); }

	inline glm::vec4 DimOverlay()    { return glm::vec4(0.12f, 0.14f, 0.18f, 0.28f); }

	inline glm::vec3 TextPrimary()   { return glm::vec3(0.22f, 0.24f, 0.28f); }
	inline glm::vec3 TextMuted()     { return glm::vec3(0.48f, 0.50f, 0.54f); }
	inline glm::vec3 TextAccent()    { return glm::vec3(0.36f, 0.46f, 0.68f); }
	inline glm::vec3 TextHighlight() { return glm::vec3(0.78f, 0.30f, 0.34f); }
	inline glm::vec3 TextSuccess()   { return glm::vec3(0.18f, 0.58f, 0.44f); }

	inline glm::vec4 BadgeFill()   { return glm::vec4(0.88f, 0.91f, 0.96f, 0.94f); }
	inline glm::vec4 BadgeBorder() { return glm::vec4(0.38f, 0.50f, 0.78f, 0.38f); }
	inline glm::vec3 BadgeText()   { return glm::vec3(0.30f, 0.40f, 0.60f); }

	inline float PanelRadius()  { return 22.0f; }
	inline float CellRadius()   { return 14.0f; }
	inline float BarRadius()    { return 18.0f; }
}

#endif
