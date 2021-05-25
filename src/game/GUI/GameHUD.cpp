#include "GameHUD.h"
#include "../Game.h"
#include "../Platform.h"
#include "../StringLib.h"
#include "../Camera.h"
#include "../MathLib.h"
#include "../Entity/PlayerSystem.h"
#include "../Entity/MapSystem.h"
#include "../Util.h"

#include "../Data/GraphicsDatabase.h"

//static Rectangle minimapDst = { {4,108},{128,128} };
static Rectangle minimapDst = { {4,124},{113,113} };

static constexpr const int MarkerTimer = 16;

GameHUD::GameHUD(const RaceDef& race, Vector2Int16 mapSize) : race(race) {

	font = Game::SystemFont;
	iconsAtlas = Game::AssetLoader.LoadAtlas("game_icons.t3x");
	minimapUpscale = Vector2(mapSize) / Vector2(minimapDst.size);

	//cmdIconsAtlas = Platform::LoadAtlas("cmdicons.t3x");
}

void GameHUD::DrawResource(Sprite icon, Vector2Int pos, Color color, const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	stbsp_vsnprintf(textBuffer, sizeof(textBuffer), fmt, args);
	va_end(args);

	Platform::Draw(icon, { pos,{ 14, 14} });
	pos += {16, -2};
	Platform::DrawText(font, pos + Vector2Int{ 1,1 }, textBuffer, Colors::Black, 0.4f);
	Platform::DrawText(font, pos, textBuffer, color, 0.4f);
}

void GameHUD::UpdateInfo(const PlayerInfo& info) {
	minerals.target = info.minerals;
	gas.target = info.gas;
	supply.current = info.GetCurrentSupply();
	supply.max = info.GetMaxSupply();
}

void GameHUD::NewActionMarker(Vector2Int16 pos)
{
	GUIActionMarker marker;
	marker.pos = pos;
	marker.timer = MarkerTimer;

	markers.clear();
	markers.push_back(marker);
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

void GameHUD::UpperScreenGUI(const Camera& camera) {

	DrawMarkers(camera);

	UpdateResourceDiff(minerals);
	UpdateResourceDiff(gas);

	Color color = Colors::UIGreen;

	// Minerals
	DrawResource(iconsAtlas->GetSprite(0), { 160, 2 }, color, "%i", minerals.shown);
	// Gas
	DrawResource(race.GasIcon, { 240, 2 }, color, "%i", gas.shown);
	// Supply
	if (supply.current > supply.max)
		color = Colors::UIRed;
	DrawResource(race.SupplyIcon, { 320,2 }, color, "%i", supply.current);


	Vector2Int pos = { 320 + 16 , 0 };
	pos.x += 7.5 * std::strlen(textBuffer);
	stbsp_snprintf(textBuffer, sizeof(textBuffer), "/%i", supply.max);
	Platform::DrawText(font, pos + Vector2Int{ 1,1 }, textBuffer, Colors::Black, 0.4f);
	Platform::DrawText(font, pos, textBuffer, Colors::UIGreen, 0.4f);

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

void GameHUD::DrawMarkers(const Camera& camera) {
	Rectangle16 camRect = camera.GetRectangle16();

	for (GUIActionMarker& marker : markers) {

		if (!camRect.Contains(marker.pos))
			continue;

		const SpriteFrame& frame = GraphicsDatabase::Cursor.targg.GetFrame(1);

		Rectangle dst = { {0,0},  Vector2Int(frame.sprite.rect.size) };


		if (marker.state == 0)
		{
			float scale = 1;
			if (marker.timer < MarkerTimer / 2) {
				scale = (float)(marker.timer) / ((float)MarkerTimer);
			}
			else {
				scale = (float)(MarkerTimer - marker.timer) / ((float)MarkerTimer);

			}
			scale = 1 + scale;
			dst.size = Vector2Int(Vector2(dst.size) * scale);
		}

		
		Vector2Int16 p = camera.WorldToScreen(marker.pos);
		dst.SetCenter(Vector2Int(p));

		Platform::Draw(frame.sprite, dst);
	}

	for (int i = 0; i < markers.size(); ++i) {
		markers[i].timer--;
		if (markers[i].timer == 0) {
			if (markers[i].state == 0) {
				++markers[i].state;
				markers[i].timer = MarkerTimer;
			}
			else {
				markers.erase(markers.begin() + i);
				--i;
			}
		}
	}
}

void GameHUD::DrawMinimap(const Camera& camera, MapSystem& mapSystem) {

	mapSystem.DrawMinimap(minimapDst);

	Rectangle camRect = camera.GetRectangle();
	Vector2 min = Vector2(camRect.GetMin());
	min /= minimapUpscale;
	min += Vector2(minimapDst.position);
	Vector2 s = Vector2(camRect.size) / minimapUpscale;

	Rectangle rect;
	rect.position = Vector2Int(min);
	rect.size = Vector2Int(s);

	Util::DrawTransparentRectangle(rect, 1, Colors::White);
}

void GameHUD::DrawAbilities() {

	//Platform::Draw(cmdIconsAtlas->GetSprite(228), { {188,120},{28,28} }, Colors::White);
	//Platform::Draw(cmdIconsAtlas->GetSprite(229), { {235,120},{28,28} }, Colors::White);
	//Platform::Draw(cmdIconsAtlas->GetSprite(230), { {280,120},{28,28} }, Colors::White);
	//Platform::Draw(cmdIconsAtlas->GetSprite(254), { {188,160},{28,28} }, Colors::White);
	//Platform::Draw(cmdIconsAtlas->GetSprite(255), { {235,160},{28,28} }, Colors::White);
	//Platform::Draw(cmdIconsAtlas->GetSprite(237), { {188,200},{28,28} }, { 0.5f,0.5f,0.5f,0.5f });
}

