#include "Cursor.h"
#include "../Platform.h"
#include "../Game.h"
#include "../Camera.h"

static constexpr const float Speed = 10;

static AnimationClip arrow, drag, illegal, magg, magr, magy, scrolld, scrolldl, scrolldr, scrolll, scrollr, scrollu, scrollul, scrollur, targg, targn, targr, targy;

static int atlasCounter = 0;

static AnimationClip* scrollAnim[9];

static void InitAnimation(const SpriteAtlas* atlas, AnimationClip& clip, int frames) {
	clip.AddSpritesFromAtlas(atlas, atlasCounter , frames);
	atlasCounter += frames;
	clip.looping = true;
	clip.frameDuration = 10;
}

Cursor::Cursor() {
	atlas = Platform::LoadAtlas("cursor.t3x");
	currentClip = &arrow;
	InitAnimation(atlas, arrow, 5);
	InitAnimation(atlas, drag, 4);
	InitAnimation(atlas, illegal, 5);
	InitAnimation(atlas, magg, 14);
	InitAnimation(atlas, magr, 14);
	InitAnimation(atlas, magy, 14);
	InitAnimation(atlas, scrolld, 2);
	InitAnimation(atlas, scrolldl, 2);
	InitAnimation(atlas, scrolldr, 2);
	InitAnimation(atlas, scrolll, 2);
	InitAnimation(atlas, scrollr, 2);
	InitAnimation(atlas, scrollu, 2);
	InitAnimation(atlas, scrollul, 2);
	InitAnimation(atlas, scrollur, 2);
	InitAnimation(atlas, targg, 2);
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

	if (clipCountdown <=0) {
		clipCountdown = currentClip->frameDuration;
		clipFrame = (++clipFrame) % currentClip->GetSpriteCount();

	}

	auto sprite = currentClip->GetSprite(clipFrame);

	Rectangle dst = { Position, sprite.rect.size };
	dst.position -= dst.size / 2;

	Platform::Draw(sprite, dst, Colors::White);
}

void Cursor::Update(Camera& camera) {
	Vector2Int corner = { 0,0 };

	if (!Game::Gamepad.L)
	{
		Vector2 move = Game::Gamepad.CPad;
		Position += move * Speed;
	}

	if (Position.x < Limits.position.x) {
		Position.x = Limits.position.x;
		corner.x = -1;
	}

	if (Position.x > Limits.position.x + Limits.size.x - 2) {
		Position.x = Limits.position.x + Limits.size.x - 2;
		corner.x = 1;
	}


	if (Position.y < Limits.position.y) {
		Position.y = Limits.position.y;
		corner.y = -1;
	}

	if (Position.y > Limits.position.y + Limits.size.y - 2) {
		Position.y = Limits.position.y + Limits.size.y - 2;
		corner.y = 1;
	}

	if (corner.LengthSquared() != 0) {
		int index = (corner.x + 1) + (corner.y + 1) * 3;
		currentClip = scrollAnim[index];
		Vector2 v = Vector2::Normalize(corner);
		camera.Position += v * camera.GetCameraSpeed();
	}
	else {
		currentClip = &arrow;
	}
}