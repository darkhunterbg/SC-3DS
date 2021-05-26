#include "GameHUD.h"
#include "../Game.h"
#include "../Platform.h"
#include "../StringLib.h"
#include "../Camera.h"
#include "../MathLib.h"
#include "../Entity/PlayerSystem.h"
#include "../Entity/MapSystem.h"
#include "../Util.h"
#include "../Entity/EntityManager.h"

#include "../Data/GraphicsDatabase.h"

#include "../Profiler.h"

//static Rectangle minimapDst = { {4,108},{128,128} };
static Rectangle minimapDst = { {4,124},{113,113} };
static Rectangle consolePanelDst= { {10,2},{220,84} };
static Rectangle commandsPanelDst = { {184,117}, {128,115} };

static constexpr const int MarkerTimer = 16;

static std::array<Color, 3> GreyHPBarColorPalette = { 0xb0b0b0ff, 0x98948cff, 0x585858ff };
static std::array<Color, 3> YellowHPBarColorPalette = { 0xfccc2cff, 0xdc9434ff, 0xb09018ff };
static std::array<Color, 3> RedHPBarColorPalette = { 0xa80808ff, 0xa80808ff, 0x840404ff };
static std::array<Color, 3> GreenHPBarColorPalette = { 0x249824ff, 0x249824ff, 0x249824ff };

GameHUD::GameHUD(const RaceDef& race, Vector2Int16 mapSize) : race(race) {

	font = Game::SystemFont;
	iconsAtlas = Game::AssetLoader.LoadAtlas("game_icons.t3x");
	minimapUpscale = Vector2(mapSize) / Vector2(minimapDst.size);


	consolePanel.Race = &race;
	consolePanel.PanelDst = consolePanelDst;

	commandsPanel.Race = &race;
	commandsPanel.PanelDst = commandsPanelDst;
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

void GameHUD::UpperScreenGUI(const Camera& camera, const std::vector<EntityId>& selection, EntityManager& em) {

	DrawMarkers(camera);
	DrawUnitBars(camera, selection, em);

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

	pos.x += Platform::MeasureString(font, textBuffer, 0.4f).x;
	stbsp_snprintf(textBuffer, sizeof(textBuffer), "/%i", supply.max);
	Platform::DrawText(font, pos + Vector2Int{ 1,1 }, textBuffer, Colors::Black, 0.4f);
	Platform::DrawText(font, pos, textBuffer, Colors::UIGreen, 0.4f);

	const auto& sprite = race.ConsoleSprite.GetSprite(1);
	Platform::Draw(sprite, { {0, 240 - sprite.rect.size.y,}, Vector2Int(sprite.rect.size) });
}

void GameHUD::LowerScreenGUI(const Camera& camera, const std::vector<EntityId>& selection, EntityManager& em) {

	Platform::Draw(race.ConsoleSprite.GetSprite(0), { {0, 0,},{ 320, 240} });

	DrawMinimap(camera, em.GetMapSystem());

	consolePanel.Draw( selection, em);

	commandsPanel.Draw(selection, em);

}

void GameHUD::ApplyInput(Camera& camera) {
	if (Game::Pointer.IsDown()) {
		if (minimapDst.Contains(Game::Pointer.Position())) {
			Vector2Int pos = Game::Pointer.Position() - Vector2Int(minimapDst.position);
			camera.Position = Vector2Int16(Vector2(pos) * minimapUpscale);
		}
	}
}

void GameHUD::UpdateSelection(std::vector<EntityId>& selection) {
	consolePanel.UpdateSelection(selection);
}

void GameHUD::DrawUnitBars(const Camera& camera, const std::vector<EntityId>& selection, EntityManager& em) {
	Rectangle16 camRect = camera.GetRectangle16();

	for (EntityId id : selection) {

		Vector2Int16 pos = em.PositionComponents.GetComponent(id);
		pos.y += em.UnitArchetype.UnitComponents.GetComponent(id).def->Graphics->Selection.BarVerticalOffset;
		int barSize = em.UnitArchetype.UnitComponents.GetComponent(id).def->Graphics->Selection.BarSize;
		int size = barSize * 3 - 1;

		Rectangle16 bb = { {0,0}, Vector2Int16(size, 5) };
		bb.SetCenter(pos);

		if (!camRect.Intersects(bb))
			continue;

		UnitHealthComponent health = em.UnitArchetype.HealthComponents.GetComponent(id);

		auto* palette = &GreenHPBarColorPalette;

		int hpBarsVisible = (int)std::ceil(((float)health.current * barSize) / (float)health.max);

		if (hpBarsVisible > 1 && hpBarsVisible == barSize && !health.AtMax()) {
			--hpBarsVisible;
		}

		Vector2Int16 dst = camera.WorldToScreen(pos);
		dst.x -= size >> 1;

		if (hpBarsVisible != barSize) {

			auto* greyPalette = &GreyHPBarColorPalette;

			Rectangle greyDst = { {Vector2Int(dst) + Vector2Int(1,1) }, {size,1 } };

			for (int i = 0; i < 3; ++i) {
				Platform::DrawRectangle(greyDst, Color32(greyPalette->at(i)));
				++greyDst.position.y;
			}


			if (health.current * 100 < health.max * 33) {
				palette = &RedHPBarColorPalette;
			}

			else if (health.current * 100 < health.max * 66) {
				palette = &YellowHPBarColorPalette;
			}
		}

		Rectangle barHp = { Vector2Int(dst) + Vector2Int(1,1), Vector2Int(hpBarsVisible * 3 - 1 ,1) };

		for (int i = 0; i < 3; ++i) {
			Platform::DrawRectangle(barHp, Color32(palette->at(i)));
			++barHp.position.y;
		}

		Rectangle16 rect = { dst,Vector2Int16(size + 2,5) };
		Util::DrawTransparentRectangle(rect, 1, Colors::Black);

		Rectangle start = { Vector2Int(dst) + Vector2Int(3,1), Vector2Int(1,3) };
	
		for (int i = 1; i < barSize; ++i) {
			Platform::DrawRectangle(start, Color32(Colors::Black));
			start.position.x += 3;
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
