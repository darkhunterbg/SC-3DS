#include "GameScene.h"
#include "Platform.h"
#include "../Map/MapSystem.h"
#include "../Game.h"

const SpriteAtlas* atlas;
const SpriteAtlas* iconsAtlas;

Font font;

GameScene::~GameScene() {
	delete mapSystem;
}

void GameScene::Start() {
	atlas = Platform::LoadAtlas("game_tconsole.t3x");
	iconsAtlas = Platform::LoadAtlas("game_icons.t3x");
	font = Platform::LoadFont("font.bcfnt");


	MapDef mapDef;
	mapDef.size = { 32,32 };

	mapSystem = new MapSystem(mapDef);
	camera.Position = { 0,0 };
	camera.Size = { 400,240 };
	camera.Limits = mapSystem->GetMapBounds();
}
void GameScene::Update() {
	
	if (Game::Pointer.Touch) {
		Rectangle rect = { {7,108}, {128,128} };
		if (rect.Contains(Game::Pointer.Position)) {
			Vector2Int pos = Game::Pointer.Position - Vector2Int(7, 108);
			camera.Position = Vector2(pos) * Vector2(8, 8);
		}
	}

	camera.Update();

}

void GameScene::Draw() {
	Platform::DrawOnScreen(ScreenId::Top);

	mapSystem->DrawTiles(camera);


	Platform::Draw(iconsAtlas->GetSprite(5), { {320, 2},{ 14, 14} }, Colors::White);
	Platform::Draw(iconsAtlas->GetSprite(2), { {240, 2},{ 14, 14} }, Colors::White);
	Platform::Draw(iconsAtlas->GetSprite(0), { {160, 2},{ 14, 14} }, Colors::White);




	Color textColor = { 0x10fc18ff };

	Platform::DrawText(font, { 177,1 }, "50", Colors::Black, 0.4f);
	Platform::DrawText(font, { 176,0 }, "50", textColor, 0.4f);

	Platform::DrawText(font, { 257,1 }, "0", Colors::Black, 0.4f);
	Platform::DrawText(font, { 256,0 }, "0", textColor, 0.4f);

	Platform::DrawText(font, { 337,1 }, "4/10", Colors::Black, 0.4f);
	Platform::DrawText(font, { 336,0 }, "4/10", textColor, 0.4f);


	Platform::DrawOnScreen(ScreenId::Bottom);
	Platform::Draw(atlas->GetSprite(0), { {0, 0,},{ 320, 240} }, Colors::White);

	mapSystem->DrawMiniMapTiles({ 7,108 }, camera);
}