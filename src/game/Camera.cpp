#include "Camera.h"

#include "Engine/InputManager.h"


void Camera::Update() {

	if (InputManager::Gamepad.IsButtonPressed(GamepadButton::R)) {
		if (Scale == 1)
			Scale = 2;
		else
			Scale = 1;
	}

	if (InputManager::Gamepad.IsButtonDown(GamepadButton::L))
	{
		Vector2 move = InputManager::Gamepad.CPad();
		move *= (int)(GetCameraSpeed());
		Position += Vector2Int16(move);
	}

	Vector2 move = InputManager::Gamepad.CStick();
	move *= (int)(GetCameraSpeed());
	Position += Vector2Int16(move);

	Vector2Int16 ScaledSize = (Size * Scale) / 2;

	if (Position.x < Limits.position.x + ScaledSize.x)
		Position.x = Limits.position.x + ScaledSize.x;

	if (Position.x > Limits.position.x + Limits.size.x - ScaledSize.x)
		Position.x = Limits.position.x + Limits.size.x - ScaledSize.x;

	if (Position.y < Limits.position.y + ScaledSize.y)
		Position.y = Limits.position.y + ScaledSize.y;

	if (Position.y > Limits.position.y + Limits.size.y - ScaledSize.y)
		Position.y = Limits.position.y + Limits.size.y - ScaledSize.y;

}