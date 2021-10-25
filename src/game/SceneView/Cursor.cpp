#include "Cursor.h"
#include "../GUI/GUI.h"
#include "../Game.h"
#include "../Data/GameDatabase.h"
#include "../MathLib.h"

static std::string _scrollAnim[] = {
	"cursor\\scrollul", "cursor\\scrollu","cursor\\scrollur",
	"cursor\\scrolll", "","cursor\\scrollr",
	"cursor\\scrolldl", "cursor\\scrolld","cursor\\scrolldr",
};

Cursor::Cursor()
{
	ChangeClip("cursor\\arrow");
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

	if (GameMode)
	{
		if (_corner.LengthSquared() != 0)
		{
			int index = (_corner.x + 1) + (_corner.y + 1) * 3;
			ChangeClip(_scrollAnim[index]);
		}
		else
		{
			ChangeClip("cursor\\arrow");
		}
	}

}

void Cursor::Draw()
{
	Update();

	auto& frame = _animation->GetFrame(_animationFrameId);
	Rectangle dst = { Position + Vector2Int(_animation->GetImageFrameOffset(frame,false)), Vector2Int(frame.size) };

	GUI::BeginAbsoluteLayout(dst);
	GUIImage::DrawAnimatedImage("cursor", *_animation, &_animationFrameId);
	GUI::EndLayout();
}
