#include "GameScene.h"

#include "../Game.h"
#include "../GUI/GameView.h"
#include "../Engine/AudioManager.h"
#include "../Entity/EntityManager.h"
#include "../Profiler.h"
#include "../Data//GameDatabase.h"

#include "../Engine/GraphicsRenderer.h"
#include "../Engine/InputManager.h"
#include "../Entity/EntityUtil.h"

#include <algorithm>

GameScene::GameScene() {}

GameScene::~GameScene() {}


void GameScene::Start()
{
	Vector2Int16 size = { 64 * 32,64 * 32 };

	camera.Position = { 0,0 };
	camera.Size = { 400,240 };
	camera.Limits = { {0,0,}, size };

	entityManager = new EntityManager();
	//entityManager->DrawBoundingBoxes = true;
	//entityManager->DrawGrid = true;
	//entityManager->DrawColliders = true;

	entityManager->Init(size);

	//view = new GameView(*entityManager, size);

	Color color[] = { Colors::SCRed, Colors::SCBlue, Colors::SCLightGreen, Colors::SCPurle,
	 Colors::SCOrange, Colors::SCGreen, Colors::SCBrown, Colors::SCLightYellow, Colors::SCWhite,
	Colors::SCTeal , Colors::SCYellow , Colors::SCLightBlue };

	int totalPlayers = 8;

	const auto& race = *GameDatabase::instance->GetRace(RaceType::Terran);

	//if (race.GetMusic())
	//	entityManager->GetSoundSystem().PlayMusic(*race.GetMusic());


	//for (int p = 0; p < totalPlayers; ++p) {
	//	entityManager->GetPlayerSystem().AddPlayer(race, color[p]);
	//}

	//view->SetPlayer(1, race);


	/*UnitEntityUtil::NewUnit(UnitDatabase::MineralField1, 0,
		Vector2Int16(128, 256));

	for (int i = 0; i < 1; ++i) {
		UnitEntityUtil::NewUnit(*UnitDatabase::Units[0], 2,
			Vector2Int16(600, 32 * i + 32));
	}

	EntityId e = UnitEntityUtil::NewUnit(UnitDatabase::CommandCenter, 1,
		Vector2Int16(256, 128));*/



		//entityManager->UnitArchetype.HealthComponents.GetComponent(e).current = 499;

		//EntityId e = UnitEntityUtil::NewUnit(*UnitDatabase::Units[0], 0,
		//	Vector2Int16(48, 48));

	const auto& def = *GameDatabase::instance->GetUnit("Terran\\Units\\Marine");

	int i = 0;
	for (int y = 0; y < 100; ++y)
	{
		for (int x = 0; x < 100; ++x)
		{
			EntityId id = entityManager->NewEntity();

			entityManager->DrawSystem.NewComponent(id);
			entityManager->DrawSystem.InitFromImage(id, def.Art.GetSprite().GetImage());
			entityManager->GetPosition(id) = Vector2Int16(Vector2Int{ x * 32 ,y * 32 });
			entityManager->DrawSystem.GetComponent(id).color = Color32(Colors::SCBlue);

			entityManager->AnimationSystem.NewComponent(id);
			EntityUtil::PlayAnimation(id, *def.Art.GetSprite().GetAnimation(AnimationType::Init), def.Art.GetShadowImage());
		}
	}

	entityManager->FullUpdate();

	/*if (loadFromFile) {
		entityManager->GetCommandProcessor().ReplayFromFile("record.cmd", *entityManager);
	}*/
}

int t = 0;

void GameScene::Update()
{

	entityManager->FrameUpdate(camera);

	t++;

	//if (t % 60 == 0) {
	//	entityManager->GetPlayerSystem().AddMinerals(1, 8);
	//	entityManager->GetPlayerSystem().AddGas(1, 8);
	//}

	//view->Update(camera);

	camera.Update();


	//if (InputManager::Gamepad.IsButtonReleased(GamepadButton::Select)) {
	//	entityManager->GetMapSystem().FogOfWarVisible = !entityManager->GetMapSystem().FogOfWarVisible;
	//}
}

void GameScene::Draw()
{
	//const PlayerInfo& playerInfo = entityManager->GetPlayerSystem().GetPlayerInfo(1);

	GraphicsRenderer::DrawOnScreen(ScreenId::Top);

	entityManager->Draw(camera);

	//view->DrawUpperScreen(camera);

	GraphicsRenderer::DrawOnScreen(ScreenId::Bottom);

	//view->DrawLowerScreen(camera);
}