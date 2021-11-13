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

	
public:
	Rectangle Limits;

	Vector2Int Position;

	bool UsingLimits = false;
	bool MultiSelectionEnabled = false;
	bool DrawMultiSelection = false;
	bool TargetMode = false;

	int CursorSpeed = 10;

	inline Vector2Int GetCornerState() const { return _corner; }
	inline bool IsHolding() const { return _hold; }
	bool GetScreenSelection(Rectangle& outRect);
	Rectangle GetHoldRect();

	Cursor();
	
	void Update();

	void GameUpdate();

	void Draw();

	void SetUnitHover(CursorHoverState state);
};