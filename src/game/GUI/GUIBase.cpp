#include "GUIBase.h"

#include "../Engine/GraphicsRenderer.h"

GUIState* GUI::_state = nullptr;

Vector2Int GUI::GetScreenSize()
{
	return Vector2Int(GraphicsRenderer::GetCurrentScreenSize());
}

void GUI::UseScreen(ScreenId screen)
{
	GraphicsRenderer::DrawOnScreen(screen);

	auto& state = GetState();
	state.SpaceStack.clear();

	state.SpaceStack.push_back({ {0,0},GetScreenSize() });
}


void GUI::BeginAbsoluteLayout(Rectangle layout)
{
	GetState().SpaceStack.push_back(layout);
}

void GUI::BeginRelativeLayout(Rectangle layout, GUIHorizontalAlignment hAlign, GUIVerticalAlignment vAlign)
{
	Rectangle space = GetState().GetSpace();

	Rectangle absLayout;
	absLayout.position = GetRelativePosition(layout, hAlign, vAlign);
	absLayout.size = layout.size;

	BeginAbsoluteLayout(absLayout);
}

void GUI::EndLayout()
{
	GetState().SpaceStack.pop_back();
}

Vector2Int GUI::GetPosition(Vector2Int pos)
{
	Rectangle space = GetState().GetSpace();
	return space.position + pos;
}

Vector2Int GUI::GetRelativePosition(Vector2Int pos, GUIHorizontalAlignment hAlign, GUIVerticalAlignment vAlign)
{
	Rectangle space = GetState().GetSpace();

	Vector2Int position = space.position;

	Vector2Int scale = { (int)hAlign, (int)vAlign };

	position += scale * space.size / 2;
	position += pos;

	return position;
}

Vector2Int GUI::GetRelativePosition(Rectangle rect, GUIHorizontalAlignment hAlign, GUIVerticalAlignment vAlign)
{
	Rectangle space = GetState().GetSpace();

	Rectangle result;

	result.position = space.position;
	result.size = rect.size;

	Vector2Int scale = { (int)hAlign, (int)vAlign };

	result.position += scale * space.size / 2;
	result.position -= scale * rect.size / 2;
	result.position += rect.position;

	return result.position;
}


Rectangle GUI::GetLayoutSpace()
{
	return GetState().GetSpace();
}