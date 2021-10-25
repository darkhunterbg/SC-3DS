#pragma once

#include <string>
#include "../Assets.h"

class Cursor {
private:
	
	const Image* _animation;
	int _animationFrameId;
	
	Vector2Int _corner;

	void Update();
	void ChangeClip(const std::string& clipName);
public:
	Vector2Int Position;

	bool GameMode = false;

	int CursorSpeed = 10;

	inline Vector2Int GetCornerState() const { return _corner; }
	Cursor();

	void Draw();
};