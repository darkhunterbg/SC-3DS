#include "GameHUD.h"
#include "../Game.h"
#include "../Platform.h"
#include "../StringLib.h"
#include "../Camera.h"
#include "../Map/MapSystem.h"

static constexpr const Color resourceTextColor = { 0x10fc18ff };
//static Rectangle minimapDst = { {7,108},{128,128} };
static Rectangle minimapDst = { {6,123},{113,113} };
static constexpr const int MinimapTextureSize = 256;

GameHUD::GameHUD() {

	minerals = 50;
	gas = 0;
	supply = { 4,10 };

	font = Game::SystemFont;
	iconsAtlas = Platform::LoadAtlas("game_icons.t3x");
	consoleAtlas = Platform::LoadAtlas("game_tconsole.t3x");
}

void GameHUD::DrawResource(Sprite icon, Vector2Int pos, const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	stbsp_vsnprintf(textBuffer, sizeof(textBuffer), fmt, args);
	va_end(args);

	Platform::Draw(icon, { pos,{ 14, 14} }, Colors::White);
	pos += {16, -2};
	Platform::DrawText(font, pos + Vector2Int{ 1,1 }, textBuffer, Colors::Black, 0.4f);
	Platform::DrawText(font, pos, textBuffer, resourceTextColor, 0.4f);
}

void GameHUD::UpperScreenGUI() {
	// Supply
	DrawResource(iconsAtlas->GetSprite(5), { 320,2 }, "%i/%i", supply.current, supply.max);
	// Gas
	DrawResource(iconsAtlas->GetSprite(2), { 240, 2 }, "%i", gas);
	// Minerals
	DrawResource(iconsAtlas->GetSprite(0), { 160, 2 }, "%i", minerals);
}

void GameHUD::LowerScreenGUI(const Camera& camera, const MapSystem& mapSystem) {

	Platform::Draw(consoleAtlas->GetSprite(0), { {0, 0,},{ 320, 240} }, Colors::White);

	DrawMinimap(camera, mapSystem);
}

void GameHUD::ApplyInput(Camera& camera) {
	if (Game::Pointer.Touch) {
		if (minimapDst.Contains(Game::Pointer.Position)) {
			Vector2Int pos = Game::Pointer.Position - Vector2Int(minimapDst.position);
			camera.Position = Vector2(pos) * minimapUpscale;
		}
	}
}

void GameHUD::DrawMinimap(const Camera& camera, const MapSystem& mapSystem) {

	if (minimapTexture == nullptr) {
		RenderMinimapTexture(mapSystem);
	}

	Platform::Draw(minimapSprite, minimapDst, Colors::White);

	Rectangle camRect = camera.GetRectangle();
	Vector2 min = camRect.GetMin();
	min /= minimapUpscale;
	Vector2 max = camRect.GetMax();
	max /= minimapUpscale;
	min += minimapDst.position;
	max += minimapDst.position;
	Vector2 s = Vector2(camRect.size) / minimapUpscale;


	Platform::DrawLine(min, min + Vector2(s.x, 0), Colors::White);
	Platform::DrawLine(max, max - Vector2(s.x, 0), Colors::White);
	Platform::DrawLine(min, min + Vector2(0, s.y), Colors::White);
	Platform::DrawLine(max,  max - Vector2(0, s.y), Colors::White);
}

void GameHUD::RenderMinimapTexture(const MapSystem& mapSystem) {
	Rectangle mapBounds = mapSystem.GetMapBounds();

	minimapTexture = Platform::NewTexture({ MinimapTextureSize,MinimapTextureSize });
	Platform::DrawOnTexture(minimapTexture);

	Vector2 upscale = Vector2(mapBounds.size) / Vector2(MinimapTextureSize, MinimapTextureSize);
	minimapUpscale = Vector2(mapBounds.size) / Vector2(minimapDst.size);

	Vector2Int tileSize = { MapTile::TileSize, MapTile::TileSize };

	for (const MapTile& tile : mapSystem.GetTiles()) {
		Vector2 pos = Vector2(tile.position * tileSize) / upscale;
		Vector2 size = Vector2(tile.sprite.rect.size) / upscale;
		Platform::Draw(tile.sprite, { pos,size }, Colors::White);
	}

	Platform::DrawOnTexture(nullptr);

	minimapSprite = { minimapTexture , {{0,0},{MinimapTextureSize,MinimapTextureSize}} };
}