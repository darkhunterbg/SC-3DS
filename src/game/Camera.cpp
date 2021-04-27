#include "Camera.h"

#include "Game.h"

static constexpr const int CameraSpeed = 10;

void Camera::Update() {

	Scale = Game::Gamepad.L ? 2 : 1;

	Vector2 move = Game::Gamepad.CPad;
	//if (move.LengthSquared() == 0)
	//	return;

	move *= CameraSpeed;

	Position += move;

	Vector2Int ScaledSize = (Size * Scale) / 2;

	if (Position.x < Limits.position.x + ScaledSize.x)
		Position.x = Limits.position.x + ScaledSize.x;

	if (Position.x > Limits.position.x + Limits.size.x - ScaledSize.x)
		Position.x = Limits.position.x + Limits.size.x - ScaledSize.x;

	if (Position.y < Limits.position.y + ScaledSize.y)
		Position.y = Limits.position.y + ScaledSize.y;

	if (Position.y > Limits.position.y + Limits.size.y - ScaledSize.y)
		Position.y = Limits.position.y + Limits.size.y - ScaledSize.y;

}