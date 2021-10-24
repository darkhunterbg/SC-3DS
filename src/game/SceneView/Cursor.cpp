#include "Cursor.h"
#include "../GUI/GUI.h"
#include "../Game.h"
#include "../Data/GameDatabase.h"
#include "../Engine/InputManager.h"

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
	_screenPosition = InputManager::Pointer.Position();

	auto& frame = _animation->GetFrame(_animationFrameId);
	Rectangle dst = { _screenPosition +  Vector2Int(_animation->GetImageFrameOffset(frame,false)), Vector2Int(frame.size) };

	GUI::BeginAbsoluteLayout(dst);
	GUIImage::DrawAnimatedImage("cursor", *_animation, &_animationFrameId);
	GUI::EndLayout();
}
