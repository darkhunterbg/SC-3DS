#pragma once

#include "GUIBase.h"

class GUILabel {
private:
	GUILabel() = delete;
	~GUILabel() = delete;
public:
	static void DrawText(const Font& font, const char* text, Vector2Int position, Color  color = Colors::White);
	static void DrawText(const Font& font, const char* text, Vector2Int position,
		GUIHorizontalAlignment hAlign,
		GUIVerticalAlignment vAlign,
		Color color = Colors::White);
};