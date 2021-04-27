#pragma once

#include "MathLib.h"

struct GamepadState {
	Vector2 CPad, CStick;
	Vector2Int DPad;
	bool L, R, ZL, ZR;
	bool A, B, X, Y;
	bool Start, Select;
};
struct PointerState {
	Vector2Int Position;
	bool Touch;
};
