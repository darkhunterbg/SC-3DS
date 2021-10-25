#include "GameSceneView.h"
#include "../Entity/EntityUtil.h"
#include "../Entity/EntityManager.h"
#include "../Data/GameDatabase.h"
#include "../Game.h"
#include "../Platform.h"
#include "../GUI/GUI.h"
#include "../StringLib.h"
#include "../Engine/AssetLoader.h"
#include "../Profiler.h"

GameSceneView::GameSceneView(GameScene* scene) : _scene(scene)
{
	Vector2Int16 size = EntityUtil::GetManager().MapSystem.GetSize();
	_camera.Position = { 0,0 };
	_camera.Limits = { {0,0,}, size };

	_cursor.GameMode = true;

	SetPlayer(EntityUtil::GetManager().MapSystem.ActivePlayer);
}

void GameSceneView::SetPlayer(PlayerId player)
{
	_player = player;
	EntityUtil::GetManager().MapSystem.ActivePlayer = player;
	const PlayerInfo& info = EntityUtil::GetManager().PlayerSystem.GetPlayerInfo(_player);

	_resourceBar.UpdatePlayerInfo(info, true);
}

const PlayerInfo& GameSceneView::GetPlayerInfo() const
{
	return EntityUtil::GetManager().PlayerSystem.GetPlayerInfo(_player);
}


void GameSceneView::Update()
{
	_camera.Size = Game::GetPlatformInfo().Screens[0];
	// Componensate for UI
	if (Game::GetPlatformInfo().Type != PlatformType::Nintendo3DS)
	{
		_camera.Size.y -= 96;
	}

	_camera.Update(&_cursor);

	const PlayerInfo& info = EntityUtil::GetManager().PlayerSystem.GetPlayerInfo(_player);

	_resourceBar.UpdatePlayerInfo(info, false);

}

void GameSceneView::Draw()
{
	//SectionProfiler p("GUI");

	DrawMainScreen();

	if (Platform::GetPlatformInfo().Type != PlatformType::Nintendo3DS)
		return;

	DrawSecondaryScreen();
}

void GameSceneView::OnPlatformChanged()
{
	GUI::UseScreen(ScreenId::Top);

	_cursor.Position = GUI::GetScreenSize() / 2;
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
		_minimap.DrawMinimap(_camera);
		GUI::EndLayout();
	}
	else
	{

		GUI::BeginRelativeLayout({ 0,0 }, Vector2Int(raceDef->ConsoleUpperSprite.size), GUIHAlign::Center, GUIVAlign::Bottom);
		GUIImage::DrawImageFrame(raceDef->ConsoleUpperSprite);
		GUI::EndLayout();
	}


	Vector2Int barSize = GUI::GetScreenSize();
	barSize.y = 14;
	GUI::BeginRelativeLayout({ 0,2 }, barSize, GUIHAlign::Center, GUIVAlign::Top);
	_resourceBar.Draw(info);
	GUI::EndLayout();

	_cursor.Draw();

}

void GameSceneView::DrawSecondaryScreen()
{
	GUI::UseScreen(ScreenId::Bottom);

	GUIImage::DrawColor(Colors::Black);

	auto& info = GetPlayerInfo();

	const RaceDef* raceDef = GameDatabase::instance->GetRace(info.race);

	if (raceDef == nullptr) raceDef = GameDatabase::instance->GetRace(RaceType::Terran);

	GUIImage::DrawImageFrame(raceDef->ConsoleLowerSprite);

	GUI::BeginRelativeLayout({ 7,-3 }, { 113,113 }, GUIHAlign::Left, GUIVAlign::Bottom);
	_minimap.DrawMinimap(_camera);
	GUI::EndLayout();
}

