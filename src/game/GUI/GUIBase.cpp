#include "GUIBase.h"

#include "../Engine/GraphicsRenderer.h"
#include "../Engine/InputManager.h"
#include "../Game.h"
#include "../Util.h"

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
void GUI::BeginRelativeLayout(Rectangle layout, GUIHAlign hAlign, GUIVAlign vAlign)
{
	Rectangle space = GetState().GetSpace();

	Rectangle absLayout;
	absLayout.position = GetPosition(layout, hAlign, vAlign);
	absLayout.size = layout.size;

	BeginAbsoluteLayout(absLayout);
}
void GUI::EndLayout()
{
	GetState().SpaceStack.pop_back();
}

Vector2Int GUI::GetPosition(Vector2Int pos, GUIHAlign hAlign, GUIVAlign vAlign)
{
	Rectangle space = GetState().GetSpace();

	Vector2Int position = space.position;

	Vector2Int scale = { (int)hAlign, (int)vAlign };

	position += scale * space.size / 2;
	position += pos + GetState().PopOffset();

	return position;
}

Vector2Int GUI::GetPosition(Rectangle rect, GUIHAlign hAlign, GUIVAlign vAlign)
{
	Rectangle space = GetState().GetSpace();

	Rectangle result;

	result.position = space.position;
	result.size = rect.size;

	Vector2Int scale = { (int)hAlign, (int)vAlign };

	result.position += scale * space.size / 2;
	result.position -= scale * rect.size / 2;
	result.position += rect.position;
	result.position += GetState().PopOffset();

	return result.position;
}


Rectangle GUI::GetLayoutSpace()
{
	Rectangle space = GetState().GetSpace();
	space.position += GetState().PopOffset();
	GetState().OverrideSize(space.size);
	return space;
}

void GUI::RegisterResource(const std::string& key, void* resource, void (*freeFunc)(void*))
{
	GetState().Resources.insert({ key, { resource, freeFunc } });
}

void GUI::CleanResources()
{
	auto& state = GetState();

	for (auto& r : state.Resources)
	{
		if (r.second.freeFunc != nullptr)
			r.second.freeFunc(r.second.memory);

	}

	state.Resources.clear();
}

void GUI::FrameEnd()
{
	for (auto& func : GetState().ExecAtEndOfFrame)
	{
		func();
	}

	GetState().ExecAtEndOfFrame.clear();
}

bool GUI::IsLayoutFocused()
{
	Rectangle layout = GetLayoutSpace();

	Vector2Int pos = GetMousePosition();

	return layout.Contains(pos);
}

bool GUI::IsLayoutActivated()
{
	if (!InputManager::Pointer.IsReleased()) return false;

	Rectangle layout = GetLayoutSpace();

	return layout.Contains(InputManager::Pointer.Position());
}
bool GUI::IsLayoutActivatedAlt()
{
	if (!InputManager::Pointer.IsReleasedAlt()) return false;

	Rectangle layout = GetLayoutSpace();

	return layout.Contains(InputManager::Pointer.Position());
}
bool GUI::IsLayoutPressed()
{
	if (!InputManager::Pointer.IsDown()) return false;

	Rectangle layout = GetLayoutSpace();

	return layout.Contains(InputManager::Pointer.Position());
}

Vector2Int GUI::GetMousePosition()
{
	if (!Game::GetInput().IsUsingMouse()) return Vector2Int{ -1000,-1000 };

	return InputManager::Pointer.Position();
}

Vector2Int GUI::GetPointerPosition()
{
	return InputManager::Pointer.Position();
}

void GUI::DrawLayoutDebug()
{
	Rectangle rect = GetLayoutSpace();

	GraphicsRenderer::DrawRectangle(rect, Color{ 1,1,1,0.3f });
	Util::DrawTransparentRectangle(rect, Colors::White);
}
