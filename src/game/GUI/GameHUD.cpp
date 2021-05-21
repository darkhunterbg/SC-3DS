#include "GameHUD.h"
#include "../Game.h"
#include "../Platform.h"
#include "../StringLib.h"
#include "../Camera.h"
#include "../MathLib.h"
#include "../Entity/PlayerSystem.h"
#include "../Entity/MapSystem.h"
#include "../Util.h"


//static Rectangle minimapDst = { {4,108},{128,128} };
static Rectangle minimapDst = { {4,124},{113,113} };



GameHUD::GameHUD(const RaceDef& race, Vector2Int16 mapSize) : race(race) {

	font = Game::SystemFont;
	iconsAtlas = Game::AssetLoader.LoadAtlas("game_icons.t3x");
	minimapUpscale = Vector2(mapSize) / Vector2(minimapDst.size);

	//cmdIconsAtlas = Platform::LoadAtlas("cmdicons.t3x");
}

void GameHUD::DrawResource(Sprite icon, Vector2Int pos, const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	stbsp_vsnprintf(textBuffer, sizeof(textBuffer), fmt, args);
	va_end(args);

	Platform::Draw(icon, { pos,{ 14, 14} });
	pos += {16, -2};
	Platform::DrawText(font, pos + Vector2Int{ 1,1 }, textBuffer, Colors::Black, 0.4f);
	Platform::DrawText(font, pos, textBuffer, Colors::UIGreen, 0.4f);
}


void GameHUD::UpdateInfo(const PlayerInfo& info) {
	minerals.target = info.minerals;
	gas.target = info.gas;
	supply.current = info.GetCurrentSupply();
	supply.max = info.GetMaxSupply();
}

static const  int GetResourceUpdate(int change) {
	return std::max(1, (int)std::ceil(std::sqrt(change)) / 2);
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
	DrawResource(race.SupplyIcon, { 320,2 }, "%i/%i", supply.current, supply.max);
	// Gas
	DrawResource(race.GasIcon, { 240, 2 }, "%i", gas.shown);
	// Minerals
	DrawResource(iconsAtlas->GetSprite(0), { 160, 2 }, "%i", minerals.shown);

	const auto& sprite = race.ConsoleSprite.GetSprite(1);
	Platform::Draw(sprite, { {0, 240 - sprite.rect.size.y,}, Vector2Int(sprite.rect.size) });
}

void GameHUD::LowerScreenGUI(const Camera& camera, MapSystem& mapSystem) {

	Platform::Draw(race.ConsoleSprite.GetSprite(0), { {0, 0,},{ 320, 240} });

	DrawMinimap(camera, mapSystem);

	DrawAbilities();
}

void GameHUD::ApplyInput(Camera& camera) {
	if (Game::Pointer.IsDown()) {
		if (minimapDst.Contains(Game::Pointer.Position())) {
			Vector2Int pos = Game::Pointer.Position() - Vector2Int(minimapDst.position);
			camera.Position = Vector2Int16(Vector2(pos) * minimapUpscale);
		}
	}
}

void GameHUD::DrawMinimap(const Camera& camera, MapSystem& mapSystem) {

	mapSystem.DrawMinimap(minimapDst);

	Rectangle camRect = camera.GetRectangle();
	Vector2 min = Vector2(camRect.GetMin());
	min /= minimapUpscale;
	Vector2 max = Vector2(camRect.GetMax());
	max /= minimapUpscale;
	min += Vector2(minimapDst.position);
	max += Vector2(minimapDst.position);
	Vector2 s = Vector2(camRect.size) / minimapUpscale;

	Rectangle rect;
	rect.position = Vector2Int(min);
	rect.size = Vector2Int(max - min);

	Util::DrawTransparentRectangle(rect, Colors::White);

	//Platform::DrawLine(Vector2Int(min), Vector2Int(min + Vector2(s.x, 0)), Colors::White);
	//Platform::DrawLine(Vector2Int(max), Vector2Int(max - Vector2(s.x, 0)), Colors::White);
	//Platform::DrawLine(Vector2Int(min), Vector2Int(min + Vector2(0, s.y)), Colors::White);
	//Platform::DrawLine(Vector2Int(max), Vector2Int(max - Vector2(0, s.y)), Colors::White);
}

void GameHUD::DrawAbilities() {

	//Platform::Draw(cmdIconsAtlas->GetSprite(228), { {188,120},{28,28} }, Colors::White);
	//Platform::Draw(cmdIconsAtlas->GetSprite(229), { {235,120},{28,28} }, Colors::White);
	//Platform::Draw(cmdIconsAtlas->GetSprite(230), { {280,120},{28,28} }, Colors::White);
	//Platform::Draw(cmdIconsAtlas->GetSprite(254), { {188,160},{28,28} }, Colors::White);
	//Platform::Draw(cmdIconsAtlas->GetSprite(255), { {235,160},{28,28} }, Colors::White);
	//Platform::Draw(cmdIconsAtlas->GetSprite(237), { {188,200},{28,28} }, { 0.5f,0.5f,0.5f,0.5f });
}

