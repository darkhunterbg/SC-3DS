#include "GameSceneView.h"
#include "../Entity/EntityUtil.h"
#include "../Entity/EntityManager.h"
#include "../Data/GameDatabase.h"
#include "../Game.h"
#include "../Platform.h"
#include "../GUI/GUI.h"
#include "../StringLib.h"
#include "../Engine/AssetLoader.h"


static const  int GetResourceUpdate(int change)
{
	return std::max(1, (int)std::ceil(std::sqrt(change)) / 2);
}

GameSceneView::GameSceneView(GameScene* scene) : _scene(scene)
{
	Vector2Int16 size = EntityUtil::GetManager().MapSystem.GetSize();
	_camera.Position = { 0,0 };
	_camera.Limits = { {0,0,}, size };

	SetPlayer(EntityUtil::GetManager().MapSystem.ActivePlayer);
}

void GameSceneView::SetPlayer(PlayerId player)
{
	_player = player;
	EntityUtil::GetManager().MapSystem.ActivePlayer = player;
	const PlayerInfo& info = EntityUtil::GetManager().PlayerSystem.GetPlayerInfo(_player);

	minerals.target = minerals.shown = info.minerals;
	gas.target = gas.shown = info.gas;
	supply.current = info.GetUsedSupply();
	supply.max = info.GetProvidedSupply();
}

const PlayerInfo& GameSceneView::GetPlayerInfo() const
{
	return EntityUtil::GetManager().PlayerSystem.GetPlayerInfo(_player);
}

void GameSceneView::UpdateResourceDiff(GameSceneView::Resource& r)
{
	int diff = r.target - r.shown;
	if (diff == 0)
		return;

	int update = GetResourceUpdate(diff);

	int mod = diff < 0 ? -update : update;
	if (std::abs(diff - mod) < update)
		mod = diff;
	r.shown += mod;
}


void GameSceneView::Update()
{
	_camera.Size = Game::GetPlatformInfo().Screens[0];
	// Componensate for UI
	if (Game::GetPlatformInfo().Type != PlatformType::Nintendo3DS)
	{
		_camera.Size.y -= 96;
	}

	_camera.Update();

	const PlayerInfo& info = EntityUtil::GetManager().PlayerSystem.GetPlayerInfo(_player);

	minerals.target = info.minerals;
	gas.target = info.gas;
	supply.current = info.GetUsedSupply();
	supply.max = info.GetProvidedSupply();

	UpdateResourceDiff(minerals);
	UpdateResourceDiff(gas);
}

void GameSceneView::DrawResource(const ImageFrame& icon, Vector2Int pos, Color color, const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	stbsp_vsnprintf(textBuffer, sizeof(textBuffer), fmt, args);
	va_end(args);


	GUI::BeginAbsoluteLayout({ pos, {14,14} });
	GUIImage::DrawImageFrame(icon);
	GUI::EndLayout();

	pos += {16, -2};

	auto font = Game::SystemFont12;

	GUILabel::DrawText(*font, textBuffer, pos + Vector2Int{ 1,1 }, Colors::Black);
	GUILabel::DrawText(*font, textBuffer, pos, color);
}

void GameSceneView::DrawMainScreen()
{
	GUI::UseScreen(ScreenId::Top);

	auto& info = GetPlayerInfo();

	const RaceDef* raceDef = GameDatabase::instance->GetRace(info.race);

	if (raceDef == nullptr) raceDef = GameDatabase::instance->GetRace(RaceType::Terran);


	if (Platform::GetPlatformInfo().Type != PlatformType::Nintendo3DS)
	{
		GUIImage::DrawImageFrame(raceDef->ConsoleSprite);

		GUI::BeginRelativeLayout({ 6,-4 }, { 128,128 }, GUIHAlign::Left, GUIVAlign::Bottom);
		DrawMinimap();
		GUI::EndLayout();
	}
	else
	{
		GUI::BeginRelativeLayout({ 0,0 }, Vector2Int(raceDef->ConsoleUpperSprite.size), GUIHAlign::Center, GUIVAlign::Bottom);
		GUIImage::DrawImageFrame(raceDef->ConsoleUpperSprite);
		GUI::EndLayout();
	}

	auto  mineralIcon = AssetLoader::GetDatabase().GetImage("game\\icons\\min").GetFrame(0);

	Color color = Colors::UIGreen;

	Vector2Int pos = GUI::GetRelativePosition({ -240,2 }, GUIHAlign::Right, GUIVAlign::Top);

	// Minerals
	DrawResource(mineralIcon, pos, color, "%i", minerals.shown);
	pos.x += 80;
	// Gas
	DrawResource(raceDef->GasIcon, pos, color, "%i", gas.shown);
	pos.x += 80;
	// Supply
	if (supply.current > supply.max)
		color = Colors::UIRed;
	DrawResource(raceDef->SupplyIcon, pos, color, "%i", supply.current);
	pos += {16, -2};

	auto font = Game::SystemFont12;

	pos.x += font->MeasureString(textBuffer).x;
	stbsp_snprintf(textBuffer, sizeof(textBuffer), "/%i", supply.max);
	GUILabel::DrawText(*font, textBuffer, pos + Vector2Int{ 1,1 }, Colors::Black);
	GUILabel::DrawText(*font, textBuffer, pos, color);


	_cursor.Draw();

}

void GameSceneView::DrawSecondaryScreen()
{
	if (Platform::GetPlatformInfo().Type != PlatformType::Nintendo3DS)
		return;

	GUI::UseScreen(ScreenId::Bottom);

	GUIImage::DrawColor(Colors::Black);

	auto& info = GetPlayerInfo();

	const RaceDef* raceDef = GameDatabase::instance->GetRace(info.race);

	if (raceDef == nullptr) raceDef = GameDatabase::instance->GetRace(RaceType::Terran);

	GUIImage::DrawImageFrame(raceDef->ConsoleLowerSprite);

	GUI::BeginRelativeLayout({ 7,-3 }, { 113,113 }, GUIHAlign::Left, GUIVAlign::Bottom);
	DrawMinimap();
	GUI::EndLayout();
}

void GameSceneView::DrawMinimap()
{
	Rectangle dst = GUI::GetLayoutSpace();
	EntityUtil::GetManager().MapSystem.DrawMinimap(dst);

	Rectangle camRect = _camera.GetRectangle();
	Vector2 min = Vector2(camRect.GetMin());

	Vector2 minimapUpscale = Vector2(EntityUtil::GetManager().MapSystem.GetSize()) / Vector2(dst.size);
	min /= minimapUpscale;
	min += Vector2(dst.position);
	Vector2 s = Vector2(camRect.size) / minimapUpscale;

	Rectangle rect;
	rect.position = Vector2Int(min) - 1;
	rect.size = Vector2Int(s) + 2;

	Util::DrawTransparentRectangle(rect, 1, Colors::White);
}