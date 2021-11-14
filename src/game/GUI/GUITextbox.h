#pragma once

#include "GUIBase.h"
#include "../Span.h"
#include "../Color.h"

class GUITextbox {

private:
	GUITextbox() = delete;
	~GUITextbox() = delete;
public:

	static bool DrawTextbox(const char* id, const class Font& font, Span<char> text, Color color = Colors::White);
};