#pragma once

#include "../Assets.h"

class Cursor {
public:
	Cursor();
	Vector2Int Position = { 0,0 };
	Rectangle Limits = { {0,0},{400,240} };
private:
	const SpriteAtlas* atlas;

public:
	void Draw();
	void Update();
};