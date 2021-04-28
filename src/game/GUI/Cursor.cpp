#include "Cursor.h"
#include "../Platform.h"
#include "../Game.h"

static constexpr const float Speed = 8;

Cursor::Cursor() {
	atlas = Platform::LoadAtlas("cursor_arrow.t3x");
}

void Cursor::Draw() {

	Rectangle dst = { Position, {atlas->GetSprite(0).rect.size} };
	dst.position -= dst.size / 2;

	Platform::Draw(atlas->GetSprite(0), dst, Colors::White);
}

void Cursor::Update() {
	if (!Game::Gamepad.L)
	{
		Vector2 move = Game::Gamepad.CPad;
		Position += move * Speed;
	}

	if (Position.x < Limits.position.x)
		Position.x = Limits.position.x;

	if (Position.x > Limits.position.x + Limits.size.x - 2)
		Position.x = Limits.position.x + Limits.size.x - 2;

	if (Position.y < Limits.position.y)
		Position.y = Limits.position.y;

	if (Position.y > Limits.position.y + Limits.size.y - 2)
		Position.y = Limits.position.y + Limits.size.y - 2;
}