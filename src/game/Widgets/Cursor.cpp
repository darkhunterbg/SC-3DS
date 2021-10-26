#include "Cursor.h"
#include "../GUI/GUI.h"
#include "../Game.h"
#include "../Data/GameDatabase.h"
#include "../MathLib.h"
#include "../Engine/GraphicsRenderer.h"
#include "../Util.h"

static std::string _scrollAnim[] = {
	"cursor\\scrollul", "cursor\\scrollu","cursor\\scrollur",
	"cursor\\scrolll", "","cursor\\scrollr",
	"cursor\\scrolldl", "cursor\\scrolld","cursor\\scrolldr",
};

Cursor::Cursor()
{
	ChangeClip("cursor\\arrow");
}

void Cursor::GameUpdate()
{
	Update();

	if (_corner.LengthSquared() != 0)
	{
		int index = (_corner.x + 1) + (_corner.y + 1) * 3;
		ChangeClip(_scrollAnim[index]);
		return;
	}

	if (!_hover && !_hold)
		ChangeClip("cursor\\arrow");

	_hover = false;
}

void Cursor::ChangeClip(const std::string& clipName)
{
	auto& newAnim = GameDatabase::instance->GetImage(clipName);
	if (&newAnim == _animation) return;

	_animation = &newAnim;
	_animationFrameId = 0;
}

void Cursor::Update()
{
	_screenSelection = { {0,0},{0,0} };

	Vector2Int screenSize = GUI::GetScreenSize();

	if (Game::GetInput().IsUsingMouse())
		Position = GUI::GetMousePosition();
	else
	{
		Position += Vector2Int(Game::GetInput().Cursor.Move.VectorValue() * CursorSpeed);

		Position.x = std::min(std::max(0, Position.x), screenSize.x);
		Position.y = std::min(std::max(0, Position.y), screenSize.y);
	}

	_corner = { 0,0 };

	if (Position.x <= 1)_corner.x = -1;
	if (Position.x >= screenSize.x - 1)_corner.x = 1;

	if (Position.y <= 1)_corner.y = -1;
	if (Position.y >= screenSize.y - 1)_corner.y = 1;

	bool holdCompleted = false;

	if (MultiSelectionEnabled)
		holdCompleted = HandleMultiselection();
	else
		_hold = false;


	if (Game::GetInput().Cursor.Select.IsActivated())
	{
		if (holdCompleted)
		{
			_screenSelection = GetHoldRect();
		}
		else
		{
			_screenSelection.position = Position;
			_screenSelection.size = { 1,1 };
		}
	}
}

Rectangle Cursor::GetHoldRect()
{
	Vector2Int size = (Position - _holdStart).Abs();
	Vector2Int center = (Position + _holdStart) / 2;

	Rectangle rect = { {0,0}, size };
	rect.SetCenter(center);

	return rect;
}

bool Cursor::HandleMultiselection()
{
	if (!_hold)
	{
		if (Game::GetInput().Cursor.Hold.IsActivated())
		{
			_hold = true;
			_holdStart = Position;
		}
	}

	if(_hold) {
		ChangeClip("cursor\\drag");

		if (!Game::GetInput().Cursor.Hold.IsActivated())
		{
			_hold = false;
			return true;
		}
	}

	return false;

}

void Cursor::Draw()
{
	if (_hold)
	{
		Rectangle rect = GetHoldRect();
		Util::DrawTransparentRectangle(rect, 2, Colors::UIGreen);
	}

	Rectangle dst = { Position - Vector2Int(_animation->GetSize() / 2), Vector2Int(_animation->GetSize()) };

	GUI::BeginAbsoluteLayout(dst);
	GUIImage::DrawAnimatedImage("cursor", *_animation, &_animationFrameId);
	GUI::EndLayout();
}

void Cursor::SetUnitHover(CursorHoverState state)
{
	if (_corner != Vector2Int{ 0, 0 } || _hold) return;

	_hover = true;

	switch (state)
	{
	case CursorHoverState::Green:
		ChangeClip("cursor\\magg"); break;
	case CursorHoverState::Yellow:
		ChangeClip("cursor\\magy"); break;
	case CursorHoverState::Red:
		ChangeClip("cursor\\magr"); break;
	default:
		break;
	}
}

bool Cursor::GetScreenSelection(Rectangle& outRect)
{
	if (_screenSelection.size.LengthSquared() > 0)
	{
		outRect = _screenSelection;
		return true;
	}
	return false;
}
