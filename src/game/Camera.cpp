#include "Camera.h"

#include "Game.h"
#include "SceneView/Cursor.h"

int Camera::GetCameraSpeed() const
{
	Vector2Int16 ScaledSize = (Size * Scale);

	return (ScaledSize.Length() * CameraSpeed) / 300;
}

void Camera::SetPositionRestricted(Vector2Int16 pos)
{
	Position = pos;

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

void Camera::Update(Cursor* cursor)
{

	if (Game::GetInput().Camera.Zoom.IsActivated())
	{
		if (Scale == 1)
			Scale = 2;
		else
			Scale = 1;
	}

	Vector2 move = Game::GetInput().Camera.Move.VectorValue();
	move *= (int)(GetCameraSpeed());
	Position += Vector2Int16(move);

	if (cursor)
	{
		Vector2 v = Vector2::Normalize(Vector2(cursor->GetCornerState()));
		Position += Vector2Int16(v * GetCameraSpeed());
	}


	SetPositionRestricted(Position);
}