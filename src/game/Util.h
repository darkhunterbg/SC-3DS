#pragma once


#include "Color.h"
#include "MathLib.h"

class Util {
private:
	Util() = delete;
	~Util() = delete;
public:
	static void DrawTransparentRectangle(const Rectangle& rect, Color c);
	static void DrawTransparentRectangle(const Rectangle16& rect, Color c);
	static void DrawTransparentRectangle(const Rectangle& rect, int thickness, Color c);
	static void DrawTransparentRectangle(const Rectangle16& rect, int thickness, Color c);

	static void RealTimeStat(const char* stat, int value);
};