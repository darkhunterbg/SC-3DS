#pragma once

#include "../Assets.h"

class Camera;

class Cursor {
public:
	Cursor();
	Vector2Int Position = { 0,0 };
	Rectangle Limits = { {0,0},{400,240} };
private:
	const SpriteAtlas* atlas;
	const AnimationClip* currentClip;
	int clipFrame = 0;
	int clipCountdown = 0;

public:
	bool isHoverState = false;
	void Draw();
	void Update(Camera& camera);
};