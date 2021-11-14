#pragma once


#include "GUIBase.h"
#include "../Assets.h"



class GUIButton {
private:
	GUIButton() = delete;
	~GUIButton() = delete;
public:

	static void SetNextButtonSelected();

	static bool DrawMainMenuButtonFromText(Vector2Int offset, GUIHAlign hAlign, GUIVAlign vAlign, const char* text, bool enabled = true);

	static bool DrawMainMenuButtonTextOffsetAligned(Vector2Int offset, GUIHAlign hAlign, GUIVAlign vAlign, const char* text, bool enabled = true);
	static bool DrawMainMenuButtonTextOffset(Vector2Int offset,  const char* text, bool enabled = true);
	static bool DrawMainMenuButton(const char* text, bool enabled = true);
	static bool DrawMainMenuButton(Rectangle dst, GUIHAlign hAlign, GUIVAlign vAlign, const char* text, bool enabled = true);
	static bool DrawMainMenuButton(Vector2Int pos, Vector2Int size, GUIHAlign hAlign, GUIVAlign vAlign, const char* text, bool enabled = true)
	{
		return DrawMainMenuButton(Rectangle { pos,size }, hAlign, vAlign, text, enabled);
	}

};