#pragma once

#include "GUIBase.h"
#include "../Assets.h"

enum class TooltipLocation {
	Right,
	Top,
};

class GUITooltip {
private:
	GUITooltip() = delete;
	~GUITooltip() = delete;
public:
	static void DrawTextTooltip(const char* id, const Font& font, const char* text, TooltipLocation location = TooltipLocation::Right);
};