#pragma once

#include "GUIBase.h"


class GUILabel {
private:
	GUILabel() = delete;
	~GUILabel() = delete;
public:
	static void DrawText(const Font& font, const char* text, Color  color = Colors::White);
	static void DrawText(const Font& font, const char* text, Vector2Int offset, Color  color = Colors::White);
	static void DrawText(const Font& font, const char* text,
		GUIHAlign hAlign,
		GUIVAlign vAlign,
		Color color = Colors::White);
	static void DrawText(const Font& font, const char* text, Vector2Int offset,
		GUIHAlign hAlign,
		GUIVAlign vAlign,
		Color color = Colors::White);
};