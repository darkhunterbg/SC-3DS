#include "GameSceneView.h"
#include "../Entity/EntityUtil.h"
#include "../Entity/EntityManager.h"
#include "../Data/GameDatabase.h"
#include "../Game.h"
#include "../Platform.h"
#include "../GUI/GUI.h"

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
}

const PlayerInfo& GameSceneView::GetPlayerInfo() const
{
	return EntityUtil::GetManager().PlayerSystem.GetPlayerInfo(_player);
}

void GameSceneView::Update()
{
	_camera.Update();
}

void GameSceneView::DrawMainScreen()
{
	_camera.Size = Game::GetPlatformInfo().Screens[0];


	//if (Game::GetPlatformInfo().PointerIsCursor)
	//{
	//	_cursor.Position = Vector2Int16(InputManager::Pointer.Position());
	//}
	//else
	//{
	//	// TODO: Instead of polling directly, use commands
	//	
	//	Vector2 move = InputManager::Gamepad.CPad();
	//	_cursor.Position += Vector2Int16(move * 10);
	//}

	GUI::UseScreen(ScreenId::Top);

	auto& info = GetPlayerInfo();

	const RaceDef* raceDef = GameDatabase::instance->GetRace(info.race);

	if (raceDef == nullptr) raceDef = GameDatabase::instance->GetRace(RaceType::Terran);

	if (Platform::GetPlatformInfo().Type != PlatformType::Nintendo3DS)
	{
		GUIImage::DrawImageFrame(raceDef->ConsoleSprite);
	}
	else
	{
		GUI::BeginRelativeLayout({ 0,0 }, Vector2Int(raceDef->ConsoleUpperSprite.size), GUIHAlign::Center, GUIVAlign::Bottom);
		GUIImage::DrawImageFrame(raceDef->ConsoleUpperSprite);
		GUI::EndLayout();
	}

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
}