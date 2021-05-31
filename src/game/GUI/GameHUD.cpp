#include "GameHUD.h"
#include "../Game.h"
#include "../StringLib.h"
#include "../Camera.h"
#include "../MathLib.h"
#include "../Entity/PlayerSystem.h"
#include "../Entity/MapSystem.h"
#include "../Util.h"
#include "../Entity/EntityManager.h"
#include "../Engine/GraphicsRenderer.h"

#include "../Data/GraphicsDatabase.h"

#include "../Profiler.h"

#include "../Platform.h"

//static Rectangle minimapDst = { {4,108},{128,128} };
static Rectangle minimapDst = { {4,124},{113,113} };
static Rectangle consolePanelDst = { {10,2},{220,84} };
static Rectangle commandsPanelDst = { {186,118}, {128,115} };
static Rectangle portraitPanelDst = { {248, 20},{ 60, 56 } };

GameHUD::GameHUD(Vector2Int16 mapSize)
{
	font = Game::SystemFont;
	iconsAtlas = Game::AssetLoader.LoadAtlas("game_icons.t3x");
	minimapUpscale = Vector2(mapSize) / Vector2(minimapDst.size);

	consolePanel.PanelDst = consolePanelDst;
	commandsPanel.PanelDst = commandsPanelDst;
}

void GameHUD::DrawResource(Sprite icon, Vector2Int pos, Color color, const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	stbsp_vsnprintf(textBuffer, sizeof(textBuffer), fmt, args);
	va_end(args);

	GraphicsRenderer::Draw(icon, { pos,{ 14, 14} });
	pos += {16, -2};
	GraphicsRenderer::DrawText(font, pos + Vector2Int{ 1,1 }, textBuffer, Colors::Black, 0.4f);
	GraphicsRenderer::DrawText(font, pos, textBuffer, color, 0.4f);
}

void GameHUD::UpdateInfo(GameViewContext& context) {
	const PlayerInfo& info = context.GetEntityManager().GetPlayerSystem().GetPlayerInfo(context.player);

	minerals.target = info.minerals;
	gas.target = info.gas;
	supply.current = info.GetUsedSupply();
	supply.max = info.GetProvidedSupply();
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

void GameHUD::UpperScreenGUI(const Camera& camera, GameViewContext& context) {

	UpdateInfo(context);

	UpdateResourceDiff(minerals);
	UpdateResourceDiff(gas);

	Color color = Colors::UIGreen;

	// Minerals
	DrawResource(iconsAtlas->GetSprite(0), { 160, 2 }, color, "%i", minerals.shown);
	// Gas
	DrawResource(context.race->GasIcon, { 240, 2 }, color, "%i", gas.shown);
	// Supply
	if (supply.current > supply.max)
		color = Colors::UIRed;
	DrawResource(context.race->SupplyIcon, { 320,2 }, color, "%i", supply.current);


	Vector2Int pos = { 320 + 16 , 0 };

	pos.x += Platform::MeasureString(font, textBuffer, 0.4f).x;
	stbsp_snprintf(textBuffer, sizeof(textBuffer), "/%i", supply.max);
	GraphicsRenderer::DrawText(font, pos + Vector2Int{ 1,1 }, textBuffer, Colors::Black, 0.4f);
	GraphicsRenderer::DrawText(font, pos, textBuffer, Colors::UIGreen, 0.4f);

	const auto& sprite = context.race->ConsoleSprite.GetSprite(1);
	GraphicsRenderer::Draw(sprite, { {0, 240 - sprite.rect.size.y,}, Vector2Int(sprite.rect.size) });

	if (context.IsTargetSelectionMode) {
		pos = { 0, 240 - sprite.rect.size.y };
		pos.y -= 16;
		pos.x += 160;
		GraphicsRenderer::DrawText(font, pos + Vector2Int{ 1, 1 }, "Select Target", Colors::Black, 0.4f);
		GraphicsRenderer::DrawText(font, pos, "Select Target", Colors::UILightGray, 0.4f);

	}
}

void GameHUD::LowerScreenGUI(const Camera& camera, GameViewContext& context) {

	GraphicsRenderer::ClearCurrentSurface({ 0x080808ff });

	EntityId selected = context.GetPriorityUnitSelected();
	if (selected != Entity::None) {
		auto& def = *context.GetEntityManager().UnitArchetype.UnitComponents.GetComponent(selected).def;
		GraphicsRenderer::Draw(def.Portrait, portraitPanelDst);
	}

	GraphicsRenderer::Draw(context.race->ConsoleSprite.GetSprite(0), { {0, 0,},{ 320, 240} });


	DrawMinimap(camera, context);

	consolePanel.Draw(context);

	commandsPanel.Draw(context);

}

void GameHUD::Update(Camera& camera, GameViewContext& context) {
	if (Game::Pointer.IsDown()) {
		if (minimapDst.Contains(Game::Pointer.Position())) {
			Vector2Int pos = Game::Pointer.Position() - Vector2Int(minimapDst.position);
			camera.Position = Vector2Int16(Vector2(pos) * minimapUpscale);
		}

		if (portraitPanelDst.Contains(Game::Pointer.Position())) {
			EntityId selected = context.GetPriorityUnitSelected();
			if (selected != Entity::None) {
				Vector2Int16 pos = context.GetEntityManager().PositionComponents.GetComponent(selected);
				camera.Position = pos;
			}
		}
	}

	consolePanel.UpdateSelection(context);
	commandsPanel.UpdateInput(context);
}

void GameHUD::DrawMinimap(const Camera& camera, GameViewContext& context) {

	context.GetEntityManager().GetMapSystem().DrawMinimap(minimapDst);

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
