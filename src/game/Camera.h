#pragma once

#include "MathLib.h"

class Camera {

public:
	Vector2Int Position;
	Vector2Int Size;
	Rectangle Limits;
	int Scale = 1;

	inline Rectangle GetRectangle() const {
		return { Position - (Size * Scale)/2, Size * Scale };
	}


	void Update();
};