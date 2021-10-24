#pragma once

#include <string>
#include "../Assets.h"

class Cursor {
private:
	Vector2Int _screenPosition;
	const Image* _animation;
	int _animationFrameId;
public:
	Cursor();

	void Draw();
};