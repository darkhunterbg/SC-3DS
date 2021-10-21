#pragma once

#include "../Assets.h"
#include "../Color.h"
#include "../MathLib.h"
#include "../Engine/GraphicsPrimitives.h"
#include <vector>

enum class GUIHorizontalAlignment {
	Left = 0,
	Center = 1,
	Right = 2,
};

enum class GUIVerticalAlignment {
	Top = 0,
	Center = 1,
	Bottom = 2,
};

class GUIState {
public:
	std::vector<Rectangle> SpaceStack;

	Rectangle GetSpace()const { return SpaceStack.back(); }
};

class GUI {
private:
	GUI() = delete;
	~GUI() = delete;

	static GUIState* _state;
public:
	static Vector2Int GetScreenSize();

	static void SetState(GUIState& state) { _state = &state; }
	static GUIState& GetState() { return *_state; }

	static void UseScreen(ScreenId screen);

	static void BeginAbsoluteLayout(Rectangle layout);
	static void BeginRelativeLayout(Rectangle layout,
		GUIHorizontalAlignment hAlign = GUIHorizontalAlignment::Center,
		GUIVerticalAlignment vAlign = GUIVerticalAlignment::Center);

	static void EndLayout();

	static Vector2Int GetPosition(Vector2Int pos);
	static Vector2Int GetRelativePosition(Vector2Int pos,
		GUIHorizontalAlignment hAlign = GUIHorizontalAlignment::Center,
		GUIVerticalAlignment vAlign = GUIVerticalAlignment::Center);

	static Vector2Int GetRelativePosition(Rectangle rect,
		GUIHorizontalAlignment hAlign = GUIHorizontalAlignment::Center,
		GUIVerticalAlignment vAlign = GUIVerticalAlignment::Center);

	static Rectangle GetLayoutSpace();
};

