#include "Cursor.h"
#include "../Engine/GraphicsRenderer.h"
#include "../Engine/AssetLoader.h"
#include "../Camera.h"
#include "../Entity/EntityManager.h"
#include "../Util.h"
#include "../Entity/EntityUtil.h"
#include "../Data/GraphicsDatabase.h"
#include "GameViewContext.h"
#include <algorithm>

#include "Game.h"

static constexpr const float Speed = 10;
static constexpr const int AnimFrameCount = 6;

Cursor::Cursor() {
	atlas = AssetLoader::LoadAtlas("cursor.t3x");
	currentClip = &GraphicsDatabase::Cursor.arrow;
}


void Cursor::Draw() {

	clipCountdown--;

	if (clipCountdown <= 0) {
		clipCountdown = AnimFrameCount;
		clipFrame = (++clipFrame) % currentClip->GetFrameCount();
	}

	const SpriteFrame& frame = currentClip->GetFrame(clipFrame);

	Rectangle dst = { Vector2Int(Position + frame.offset), Vector2Int(frame.sprite.rect.size) };

	if (regionRect.size.LengthSquaredInt() > 0) {

		regionRect.position -= {1, 1};
		regionRect.size += {2, 2};

		Util::DrawTransparentRectangle(regionRect, 2, Colors::UIGreen);
	}

	GraphicsRenderer::Draw(frame.sprite, dst);
}

void Cursor::Update(Camera& camera, GameViewContext& context) {

	corner = { 0,0 };

	// TODO: State machine

	if (!Game::Gamepad.IsButtonDown(GamepadButton::L))
	{
		Vector2 move = Game::Gamepad.CPad();
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


	holding = Game::Gamepad.IsButtonDown(GamepadButton::Y);

	if (Position.y <= Limits.position.y) {
		Position.y = Limits.position.y;
		corner.y = -1;
	}

	if (Position.y >= Limits.position.y + Limits.size.y) {
		Position.y = Limits.position.y + Limits.size.y;
		corner.y = 1;
	}

	if (Game::Gamepad.IsButtonPressed(GamepadButton::Y)) {
		holdStart = camera.ScreenToWorld(Position);
	}

	worldPos = camera.ScreenToWorld(Position);
	hover = context.GetUnitAtPosition(worldPos);


	dragging = (worldPos - holdStart).LengthSquared() != 0;

	newClip = currentClip;

	if (context.IsTargetSelectionMode) {
		UpdateTargetSelectionState(camera, context);
	}
	else {
		UpdateDefaultState(camera, context);
	}

	if (corner.LengthSquared() != 0) {
		int index = (corner.x + 1) + (corner.y + 1) * 3;
		newClip = GraphicsDatabase::Cursor.scrollAnim[index];
		Vector2 v = Vector2::Normalize(Vector2(corner));
		camera.Position += Vector2Int16(v * camera.GetCameraSpeed());
	}

	if (newClip != currentClip) {
		currentClip = newClip;
		clipFrame = 0;
		clipCountdown = AnimFrameCount;
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

	if (Game::Gamepad.IsButtonReleased(GamepadButton::Y)) {
	
		if (dragging ) {
		
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
				newClip = &GraphicsDatabase::Cursor.arrow;
			}
			else {
				if (UnitEntityUtil::IsAlly(context.player, hover)) {
					newClip = &GraphicsDatabase::Cursor.magg;
				}
				else if (UnitEntityUtil::IsEnemy(context.player, hover)) {
					newClip = &GraphicsDatabase::Cursor.magr;
				}
				else {
					newClip = &GraphicsDatabase::Cursor.magy;
				}

			}
		}
		else {
			newClip = &GraphicsDatabase::Cursor.drag;

		}
	}
}

void Cursor::UpdateTargetSelectionState(Camera& camera, GameViewContext& context)
{
	if (hover == Entity::None) {
		newClip = &GraphicsDatabase::Cursor.targn;
	}
	else {
		if (UnitEntityUtil::IsAlly(context.player, hover)) {
			newClip = &GraphicsDatabase::Cursor.targg;
		}
		else if (UnitEntityUtil::IsEnemy(context.player, hover)) {
			newClip = &GraphicsDatabase::Cursor.targr;
		}
		else {
			newClip = &GraphicsDatabase::Cursor.targy;
		}

	}
}