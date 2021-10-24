#include "Cursor.h"
#include "../GUI/GUI.h"
#include "../Game.h"
#include "../Data/GameDatabase.h"

static std::string _scrollAnim[] = {
	"cursor\\scrollul", "cursor\\scrollu","cursor\\scrollur",
	"cursor\\scrolll", "","cursor\\scrollr",
	"cursor\\scrolldl", "cursor\\scrolld","cursor\\scrolldr",
};

Cursor::Cursor()
{
	_animation = &GameDatabase::instance->GetImage("cursor\\arrow");
	_animationFrameId = 0;
}

void Cursor::Draw()
{
	if (Game::GetInput().IsUsingMouse())
		_screenPosition = GUI::GetMousePosition();
	else
		_screenPosition += Vector2Int(Game::GetInput().Cursor.Move.VectorValue() * 10);

	auto& frame = _animation->GetFrame(_animationFrameId);
	Rectangle dst = { _screenPosition + Vector2Int(_animation->GetImageFrameOffset(frame,false)), Vector2Int(frame.size) };

	GUI::BeginAbsoluteLayout(dst);
	GUIImage::DrawAnimatedImage("cursor", *_animation, &_animationFrameId);
	GUI::EndLayout();
}
