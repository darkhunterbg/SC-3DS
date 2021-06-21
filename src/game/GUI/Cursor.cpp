#include "Cursor.h"
#include "../Engine/GraphicsRenderer.h"
#include "../Engine/AssetLoader.h"
#include "../Camera.h"
#include "../Entity/EntityManager.h"
#include "../Util.h"
#include "../Entity/EntityUtil.h"
#include "GameViewContext.h"
#include <algorithm>

#include "Game.h"
#include "Engine/InputManager.h"

#include "../Data/GameDatabase.h"

static constexpr const float Speed = 10;
static constexpr const int AnimFrameCount = 6;

static std::string scrollAnim[] = {
	"cursor\\scrollul", "cursor\\scrollu","cursor\\scrollur",
	"cursor\\scrolll", "","cursor\\scrollr",
	"cursor\\scrolldl", "cursor\\scrolld","cursor\\scrolldr",
};

Cursor::Cursor() {
	currentClip = "cursor\\arrow";
	animator.SetAnimationFromImage(currentClip, AnimFrameCount);
}


void Cursor::Draw() {

	animator.Update();

	const ImageFrame& frame = animator.GetImageFrame();

	Rectangle dst = { Vector2Int(Position + animator.GetImageFrameOffset()), Vector2Int( frame.size )};

	if (regionRect.size.LengthSquaredInt() > 0) {

		regionRect.position -= {1, 1};
		regionRect.size += {2, 2};

		Util::DrawTransparentRectangle(regionRect, 2, Colors::UIGreen);
	}

	GraphicsRenderer::Draw(frame, dst);
}

void Cursor::Update(Camera& camera, GameViewContext& context) {

	corner = { 0,0 };

	// TODO: State machine

	if (!InputManager::Gamepad.IsButtonDown(GamepadButton::L))
	{
		Vector2 move = InputManager::Gamepad.CPad();
		Position += Vector2Int16(move * Speed);
	}

	if (Position.x <= Limits.position.x) {
		Position.x = Limits.position.x;
		corner.x = -1;
	}

	if (Position.x >= Limits.position.x + Limits.size.x) {
		Position.x = Limits.position.x + Limits.size.x;
		corner.x = 1;
	}


	holding = InputManager::Gamepad.IsButtonDown(GamepadButton::Y);

	if (Position.y <= Limits.position.y) {
		Position.y = Limits.position.y;
		corner.y = -1;
	}

	if (Position.y >= Limits.position.y + Limits.size.y) {
		Position.y = Limits.position.y + Limits.size.y;
		corner.y = 1;
	}

	if (InputManager::Gamepad.IsButtonPressed(GamepadButton::Y)) {
		holdStart = camera.ScreenToWorld(Position);
	}

	worldPos = camera.ScreenToWorld(Position);
	hover = context.GetUnitAtPosition(worldPos);


	dragging = (worldPos - holdStart).LengthSquared() != 0;

	//newClip = currentClip;

	if (context.IsTargetSelectionMode) {
		UpdateTargetSelectionState(camera, context);
	}
	else {
		UpdateDefaultState(camera, context);
	}

	if (corner.LengthSquared() != 0) {
		int index = (corner.x + 1) + (corner.y + 1) * 3;
		newClip = scrollAnim[index];
		Vector2 v = Vector2::Normalize(Vector2(corner));
		camera.Position += Vector2Int16(v * camera.GetCameraSpeed());
	}

	if (newClip != currentClip) {
		currentClip = newClip;
		animator.SetAnimationFromImage(newClip, AnimFrameCount);
	}
}

void Cursor::UpdateDefaultState(Camera& camera, GameViewContext& context)
{
	EntityManager& em = context.GetEntityManager();

	if (holding && dragging) {
		Vector2Int16 start = camera.WorldToScreen(holdStart);
		Vector2Int16 end = camera.WorldToScreen(worldPos);

		regionRect.size = (end - start);
		regionRect.size.x = std::abs(regionRect.size.x);
		regionRect.size.y = std::abs(regionRect.size.y);
		regionRect.position.x = std::min(start.x, end.x);
		regionRect.position.y = std::min(start.y, end.y);
	}

	if (InputManager::Gamepad.IsButtonReleased(GamepadButton::Y)) {

		if (dragging) {

			Vector2Int16 start = holdStart;
			Vector2Int16 end = worldPos;

			Rectangle16 rect;

			rect.size = (end - start);
			rect.size.x = std::abs(rect.size.x);
			rect.size.y = std::abs(rect.size.y);
			rect.position.x = std::min(start.x, end.x);
			rect.position.y = std::min(start.y, end.y);

			context.SelectUnitsInRegion(rect);

			holdStart = { 0,0 };
			regionRect = { {0,0},{0,0} };

		}
		else {
			context.SelectUnitAtPosition(worldPos);
		}
	}

	if (corner == Vector2Int{ 0,0 })
	{
		if (!holding || !dragging)
		{
			if (hover == Entity::None) {
				newClip = "cursor\\arrow";
			}
			else {
				if (UnitEntityUtil::IsAlly(context.player, hover)) {
					newClip = "cursor\\magg";
				}
				else if (UnitEntityUtil::IsEnemy(context.player, hover)) {
					newClip = "cursor\\magr";
				}
				else {
					newClip = "cursor\\magy";
				}

			}
		}
		else {
			newClip = "cursor\\drag";

		}
	}
}

void Cursor::UpdateTargetSelectionState(Camera& camera, GameViewContext& context)
{
	if (hover == Entity::None) {
		newClip = "cursor\\targn";
	}
	else {
		if (UnitEntityUtil::IsAlly(context.player, hover)) {
			newClip = "cursor\\targg";
		}
		else if (UnitEntityUtil::IsEnemy(context.player, hover)) {
			newClip = "cursor\\targr";
		}
		else {
			newClip = "cursor\\targy";
		}

	}
}