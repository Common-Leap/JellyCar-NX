#ifndef UiLayout_H
#define UiLayout_H

namespace UiLayout
{
	constexpr float TitleFontSize = 64.0f;
	constexpr float MenuFontSize  = 32.0f;
	constexpr float SmallFontSize = 16.0f;

	inline int CenterX(int x, int w) { return x + w / 2; }
	inline int CenterY(int y, int h) { return y + h / 2; }

	// freetype_gl Y is the glyph baseline
	inline int BaselineCenter(int boxY, int boxH, float fontSize)
	{
		return boxY + boxH / 2 + (int)(fontSize * 0.30f);
	}

	inline int BaselineRow(int boxY, int boxH, int row, int rowCount, float fontSize)
	{
		float rowH = (float)boxH / (float)rowCount;
		float rowCenterY = boxY + rowH * (row + 0.5f);
		return (int)(rowCenterY + fontSize * 0.30f);
	}

	inline int BaselineInPanel(int panelY, int panelH, float fontSize)
	{
		return panelY + panelH - (int)(fontSize * 0.22f);
	}

	// Center a line in the gap between the top of a box and another baseline below it
	inline int BaselineBetweenTopAndLine(int boxY, int lowerBaseline, float fontSize)
	{
		int topAnchor = boxY + (int)(fontSize * 0.20f);
		return topAnchor + (lowerBaseline - topAnchor) / 2 + (int)(fontSize * 0.10f);
	}
}

#endif
