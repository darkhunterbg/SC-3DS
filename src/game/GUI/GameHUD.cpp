#include "GameHUD.h"
#include "../Game.h"
#include "../Platform.h"
#include "../StringLib.h"
#include "../Camera.h"
#include "../Map/MapSystem.h"
#include "../MathLib.h"


//static Rectangle minimapDst = { {7,108},{128,128} };
static Rectangle minimapDst = { {6,123},{113,113} };
static constexpr const int MinimapTextureSize = 256;


GameHUD::GameHUD() {

	font = Game::SystemFont;
	iconsAtlas = Platform::LoadAtlas("game_icons.t3x");
	consoleAtlas = Platform::LoadAtlas("game_tconsole.t3x");
	//cmdIconsAtlas = Platform::LoadAtlas("cmdicons.t3x");
}

void GameHUD::DrawResource(Sprite icon, Vector2Int pos, const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	stbsp_vsnprintf(textBuffer, sizeof(textBuffer), fmt, args);
	va_end(args);

	Platform::Draw(icon, { pos,{ 14, 14} }, Colors::White);
	pos += {16, -2};
	Platform::DrawText(font, pos + Vector2Int{ 1,1 }, textBuffer, Colors::Black, 0.4f);
	Platform::DrawText(font, pos, textBuffer, Colors::UIGreen, 0.4f);
}

static const  int GetResourceUpdate(int change) {
	return std::max(1, (int)std::ceil(std::sqrt(change))/2 );
}

void GameHUD::UpdateResourceDiff(GameHUD::Resource& r) {
	int diff = r.target - r.shown;
	if (diff == 0)
		return;

	int update = GetResourceUpdate(diff);

	int mod = diff < 0 ? -update : update;
	if (std::abs(diff - mod) < update)
		mod = diff;
	r.shown += mod;
}

void GameHUD::UpperScreenGUI() {

	UpdateResourceDiff(minerals);
	UpdateResourceDiff(gas);

	// Supply
	DrawResource(iconsAtlas->GetSprite(5), { 320,2 }, "%i/%i", supply.current, supply.max);
	// Gas
	DrawResource(iconsAtlas->GetSprite(2), { 240, 2 }, "%i", gas.shown);
	// Minerals
	DrawResource(iconsAtlas->GetSprite(0), { 160, 2 }, "%i", minerals.shown);
}

void GameHUD::LowerScreenGUI(const Camera& camera, const MapSystem& mapSystem) {

	Platform::Draw(consoleAtlas->GetSprite(0), { {0, 0,},{ 320, 240} }, Colors::White);

	DrawMinimap(camera, mapSystem);

	DrawAbilities();
}

void GameHUD::ApplyInput(Camera& camera) {
	if (Game::Pointer.IsDown()) {
		if (minimapDst.Contains(Game::Pointer.Position())) {
			Vector2Int pos = Game::Pointer.Position() - Vector2Int(minimapDst.position);
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
	Platform::DrawLine(max, max - Vector2(0, s.y), Colors::White);
}

void GameHUD::DrawAbilities() {

	//Platform::Draw(cmdIconsAtlas->GetSprite(228), { {188,120},{28,28} }, Colors::White);
	//Platform::Draw(cmdIconsAtlas->GetSprite(229), { {235,120},{28,28} }, Colors::White);
	//Platform::Draw(cmdIconsAtlas->GetSprite(230), { {280,120},{28,28} }, Colors::White);
	//Platform::Draw(cmdIconsAtlas->GetSprite(254), { {188,160},{28,28} }, Colors::White);
	//Platform::Draw(cmdIconsAtlas->GetSprite(255), { {235,160},{28,28} }, Colors::White);
	//Platform::Draw(cmdIconsAtlas->GetSprite(237), { {188,200},{28,28} }, { 0.5f,0.5f,0.5f,0.5f });
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