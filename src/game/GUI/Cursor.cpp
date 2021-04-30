#include "Cursor.h"
#include "../Platform.h"
#include "../Game.h"
#include "../Camera.h"

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


	Rectangle dst = { Position + frame.offset,frame.sprite.rect.size };
	dst.position -= {64, 64};
	//dst.position -= dst.size / 2;

	Platform::Draw(frame.sprite, dst, Colors::White);
}

void Cursor::Update(Camera& camera) {

	Vector2Int corner = { 0,0 };

	if (!Game::Gamepad.L)
	{
		Vector2 move = Game::Gamepad.CPad;
		Position += move * Speed;
	}

	if (Position.x <= Limits.position.x) {
		Position.x = Limits.position.x;
		corner.x = -1;
	}

	if (Position.x >= Limits.position.x + Limits.size.x) {
		Position.x = Limits.position.x + Limits.size.x;
		corner.x = 1;
	}


	if (Position.y <= Limits.position.y) {
		Position.y = Limits.position.y;
		corner.y = -1;
	}

	if (Position.y >= Limits.position.y + Limits.size.y) {
		Position.y = Limits.position.y + Limits.size.y;
		corner.y = 1;
	}

	const auto* newClip = currentClip;

	if (corner.LengthSquared() != 0) {
		int index = (corner.x + 1) + (corner.y + 1) * 3;
		newClip = scrollAnim[index];
		Vector2 v = Vector2::Normalize(corner);
		camera.Position += v * camera.GetCameraSpeed();
	}
	else {
		newClip = isHoverState ? &magg : &arrow;
	}

	if (newClip != currentClip) {
		currentClip = newClip;
		clipFrame = 0;
		clipCountdown = currentClip->frameDuration;
	}
}