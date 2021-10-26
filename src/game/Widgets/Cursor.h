#pragma once

#include <string>
#include "../Assets.h"

enum class CursorHoverState {
	Green,
	Yellow,
	Red
};

class Cursor {
private:
	
	const Image* _animation;
	int _animationFrameId;
	
	Vector2Int _corner;
	bool _hover = false;
	bool _hold = false;

	bool HandleMultiselection();
	void ChangeClip(const std::string& clipName);

	Rectangle _screenSelection = { {0,0}, {0,0} };
	Vector2Int _holdStart = { 0,0 };

	Rectangle GetHoldRect();
public:
	Vector2Int Position;

	bool MultiSelectionEnabled = false;

	int CursorSpeed = 8;

	inline Vector2Int GetCornerState() const { return _corner; }
	bool GetScreenSelection(Rectangle& outRect);

	Cursor();
	
	void Update();

	void GameUpdate();

	void Draw();

	void SetUnitHover(CursorHoverState state);
};