#include "Cursor.h"
#include "../Platform.h"
#include "../Game.h"
#include "../Camera.h"
#include "../Entity/EntityManager.h"

static constexpr const float Speed = 10;

static AnimationClip arrow, drag, illegal, magg, magr, magy, scrolld, scrolldl, scrolldr, scrolll, scrollr, scrollu, scrollul, scrollur, targg, targn, targr, targy;

static int atlasCounter = 0;

static AnimationClip* scrollAnim[9];

static void InitAnimation(const SpriteAtlas* atlas, AnimationClip& clip, int frames, Vector2Int offset = { 0,0 }) {
	clip.AddSpritesFromAtlas(atlas, atlasCounter, frames, offset);
	atlasCounter += frames;
	clip.looping = true;
	clip.frameDuration = 10;
}

Cursor::Cursor() {
	atlas = Platform::LoadAtlas("cursor.t3x");
	currentClip = &arrow;
	InitAnimation(atlas, arrow, 5, { 63,63 });
	InitAnimation(atlas, drag, 4, { 55,53 });
	InitAnimation(atlas, illegal, 5, { 56,60 });
	InitAnimation(atlas, magg, 14);
	InitAnimation(atlas, magr, 14);
	InitAnimation(atlas, magy, 14);

	for (int i = 0; i < 4; ++i) {
		magg.SetFrameOffset(i, { 44,51 });
	}

	magg.SetFrameOffset(4, { 48,51 });

	for (int i = 5; i < 7; ++i) {
		magg.SetFrameOffset(i, { 51,51 });
	}
	magg.SetFrameOffset(7, { 51,52 });
	magg.SetFrameOffset(8, { 48,55 });

	for (int i = 9; i < 11; ++i) {
		magg.SetFrameOffset(i, { 44,56 });
	}

	magg.SetFrameOffset(11, { 44,55 });
	magg.SetFrameOffset(12, { 44,52 });
	magg.SetFrameOffset(13, { 44,51 });

	InitAnimation(atlas, scrolld, 2, { 46,43 });
	InitAnimation(atlas, scrolldl, 2, { 63,36 });
	InitAnimation(atlas, scrolldr, 2, { 35,36 });
	InitAnimation(atlas, scrolll, 2, { 63,46 });
	InitAnimation(atlas, scrollr, 2, { 43,45 });
	InitAnimation(atlas, scrollu, 2, { 45,63 });
	InitAnimation(atlas, scrollul, 2, { 63,63 });
	InitAnimation(atlas, scrollur, 2, { 35,63 });
	InitAnimation(atlas, targg, 2);
	InitAnimation(atlas, targn, 1, { 51,57 });
	InitAnimation(atlas, targr, 2);
	InitAnimation(atlas, targy, 2);

	scrollAnim[0] = &scrollul;
	scrollAnim[1] = &scrollu;
	scrollAnim[2] = &scrollur;
	scrollAnim[3] = &scrolll;
	scrollAnim[4] = nullptr;;
	scrollAnim[5] = &scrollr;
	scrollAnim[6] = &scrolldl;
	scrollAnim[7] = &scrolld;
	scrollAnim[8] = &scrolldr;


}

void Cursor::Draw() {

	clipCountdown--;

	if (clipCountdown <= 0) {
		clipCountdown = currentClip->frameDuration;
		clipFrame = (++clipFrame) % currentClip->GetFrameCount();
	}

	const SpriteFrame& frame = currentClip->GetFrame(clipFrame);

	Rectangle dst = { Vector2Int(Position) + frame.offset,frame.sprite.rect.size };
	dst.position -= {64, 64};

	if (regionRect.size.LengthSquared() > 0) {
		Rectangle rect;
		rect.position = regionRect.position;
		rect.size = Vector2Int(regionRect.size.x + 1, 2);
		Platform::DrawRectangle(rect, Colors::UIGreen);

		rect.size = Vector2Int(2, regionRect.size.y + 1);
		Platform::DrawRectangle(rect, Colors::UIGreen);


		rect.position = { regionRect.position.x , regionRect.GetMax().y - 1 };
		rect.size = Vector2Int(regionRect.size.x, 2);
		Platform::DrawRectangle(rect, Colors::UIGreen);

		rect.position = { regionRect.GetMax().x - 1, regionRect.position.y };
		rect.size = Vector2Int(2, regionRect.size.y);
		Platform::DrawRectangle(rect, Colors::UIGreen);
	}

	Platform::Draw(frame.sprite, dst);
}

void Cursor::Update(Camera& camera, EntityManager& entityManager, std::vector<EntityId>& outSelection) {

	Vector2Int corner = { 0,0 };

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


	bool holding = Game::Gamepad.IsButtonDown(GamepadButton::A);

	if (Position.y <= Limits.position.y) {
		Position.y = Limits.position.y;
		corner.y = -1;
	}

	if (Position.y >= Limits.position.y + Limits.size.y) {
		Position.y = Limits.position.y + Limits.size.y;
		corner.y = 1;
	}

	if (Game::Gamepad.IsButtonPressed(GamepadButton::A)) {
		holdStart = camera.ScreenToWorld(Position);
	}

	Vector2Int16 worldPos = camera.ScreenToWorld(Position);
	EntityId hover = entityManager.PointCast(worldPos);
	bool dragging = (worldPos - holdStart).LengthSquared() != 0;

	if (holding && dragging) {
		Vector2Int16 start = camera.WorldToScreen(holdStart);
		Vector2Int16 end = camera.WorldToScreen(worldPos);

		regionRect.size = (end - start);
		regionRect.size.x = std::abs(regionRect.size.x);
		regionRect.size.y = std::abs(regionRect.size.y);
		regionRect.position.x = std::min(start.x, end.x);
		regionRect.position.y = std::min(start.y, end.y);
	}

	if (Game::Gamepad.IsButtonReleased(GamepadButton::A)) {
		if (!dragging && hover) {
			outSelection.push_back(hover);
		}
		else {
			Vector2Int16 start = holdStart;
			Vector2Int16 end = worldPos;

			Rectangle16 rect;

			rect.size = (end - start);
			rect.size.x = std::abs(rect.size.x);
			rect.size.y = std::abs(rect.size.y);
			rect.position.x = std::min(start.x, end.x);
			rect.position.y = std::min(start.y, end.y);

			entityManager.RectCast(rect, outSelection);
			holdStart = { 0,0 };
			regionRect = { {0,0},{0,0} };

		}
	}

	const auto* newClip = currentClip;

	if (corner.LengthSquared() != 0) {
		int index = (corner.x + 1) + (corner.y + 1) * 3;
		newClip = scrollAnim[index];
		Vector2 v = Vector2::Normalize(Vector2(corner));
		camera.Position += Vector2Int16(v * camera.GetCameraSpeed());
	}
	else {
		if (!holding || !dragging)
		{
			newClip = hover ? &magg : &arrow;
		}
		else {
			newClip = &drag;

		}
	}

	if (newClip != currentClip) {
		currentClip = newClip;
		clipFrame = 0;
		clipCountdown = currentClip->frameDuration;
	}

}