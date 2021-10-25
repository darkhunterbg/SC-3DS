#include "Cursor.h"
#include "../GUI/GUI.h"
#include "../Game.h"
#include "../Data/GameDatabase.h"
#include "../MathLib.h"
#include "../Engine/GraphicsRenderer.h"

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

	if (!_hover)
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

}

void Cursor::Draw()
{
	Update();

	//auto& frame = _animation->GetFrame(_animationFrameId);
	Rectangle dst = { Position - Vector2Int(_animation->GetSize()/2), Vector2Int(_animation->GetSize()) };

	GUI::BeginAbsoluteLayout(dst);
	GUIImage::DrawAnimatedImage("cursor", *_animation, &_animationFrameId);
	GUI::EndLayout();
}

void Cursor::SetUnitHover(CursorHoverState state)
{
	if (_corner != Vector2Int{ 0, 0 }) return;

	_hover = true;

	switch (state)
	{
	case Green:
		ChangeClip("cursor\\magg"); break;
	case Yellow:
		ChangeClip("cursor\\magy"); break;
	case Red:
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
