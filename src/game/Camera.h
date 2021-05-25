#pragma once

#include "MathLib.h"

class Camera {
	static constexpr const int CameraSpeed = 10;
public:
	Vector2Int16 Position;
	Vector2Int16 Size;
	Rectangle16 Limits;
	int Scale = 1;

	inline int GetCameraSpeed() const { return CameraSpeed * Scale; }

	inline Rectangle16 GetRectangle16() const {
		return Rectangle16((Position - (Size * Scale) / 2), Size * Scale);
	}

	inline Rectangle GetRectangle() const {
		return Rectangle{ Vector2Int( Position - (Size * Scale) / 2), Vector2Int( Size * Scale) };
	}

	inline Vector2Int16 ScreenToWorld(Vector2Int16 p) const {
		return p * Scale + Position - (Size * Scale) / 2;
	}
	inline Vector2Int16 WorldToScreen(Vector2Int16 p) const {
		return (p - Position + (Size * Scale) / 2) / Scale;
	}

	void Update();
};