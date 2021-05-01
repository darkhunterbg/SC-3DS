#pragma once

#include "MathLib.h"

class Camera {
	static constexpr const int CameraSpeed = 10;
public:
	Vector2Int Position;
	Vector2Int Size;
	Rectangle Limits;
	int Scale = 1;

	inline int GetCameraSpeed() const { return CameraSpeed * Scale; }

	inline Rectangle GetRectangle() const {
		return { Position - (Size * Scale)/2, Size * Scale };
	}

	inline Vector2Int ScreenToWorld(Vector2Int p) {
		return p * Scale + Position - (Size * Scale) / 2;
	}
	inline Vector2Int WorldToScreen(Vector2Int p) {
		return (p - Position + (Size * Scale) / 2) / Scale;
	}

	void Update();
};