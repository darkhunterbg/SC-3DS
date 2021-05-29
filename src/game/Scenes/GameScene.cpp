#include "GameScene.h"

#include "../Game.h"
#include "../GUI/GameView.h"
#include "../Audio.h"
#include "../Entity/EntityManager.h"
#include "../Profiler.h"
#include "../Data/UnitDatabase.h"
#include "../Data/RaceDatabase.h"
#include "../Entity/EntityUtil.h"

#include <algorithm>

GameScene::GameScene() {}

GameScene::~GameScene() {}


void GameScene::Start() {
	Vector2Int16 size = { 64 * 32,64 * 32 };

	camera.Position = { 0,0 };
	camera.Size = { 400,240 };
	camera.Limits = { {0,0,}, size };

	UnitDatabase::LoadAllUnitResources();

	entityManager = new EntityManager();
	//entityManager->DrawBoundingBoxes = true;
	//entityManager->DrawGrid = true;
	//entityManager->DrawColliders = true;

	entityManager->Init(size);

	view = new GameView(*entityManager, size);

	Color color[] = { Colors::SCRed, Colors::SCBlue, Colors::SCLightGreen, Colors::SCPurle,
	 Colors::SCOrange, Colors::SCGreen, Colors::SCBrown, Colors::SCLightYellow, Colors::SCWhite,
	Colors::SCTeal , Colors::SCYellow , Colors::SCLightBlue };

	int totalPlayers = 8;

	auto& race = RaceDatabase::Terran;
	race.LoadResourses();


	AudioStream* stream = race.GameMusic[std::rand() % race.GameMusic.size()].Stream;
	Game::Audio.PlayStream(stream, 0);

	for (int p = 0; p < totalPlayers; ++p) {
		entityManager->GetPlayerSystem().AddPlayer(race, color[p]);
	}

	view->SetPlayer(1, race);


	UnitEntityUtil::NewUnit(UnitDatabase::MineralField1, 0,
		Vector2Int16(128, 256));

	for (int i = 0; i < 1; ++i) {
		UnitEntityUtil::NewUnit(*UnitDatabase::Units[0], 2,
			Vector2Int16(600, 32 * i + 32));
	}

	EntityId e = UnitEntityUtil::NewUnit(UnitDatabase::CommandCenter, 1,
		Vector2Int16(256, 128));

	//entityManager->UnitArchetype.HealthComponents.GetComponent(e).current = 499;

	//EntityId e = UnitEntityUtil::NewUnit(*UnitDatabase::Units[0], 0,
	//	Vector2Int16(48, 48));

	int i = 0;
	for (int y = 1; y > 0; --y) {
		for (int x = 1; x > 0; --x) {
			Color c = color[(i) % 12];
			auto& def = *UnitDatabase::Units[0];
			 UnitEntityUtil::NewUnit(def, 1 + i / 200,// 1 + i % totalPlayers,
				Vector2Int16(Vector2Int{ x * 32 + 48,y * 32 + 48 }));

			i += 1;

		}
	}

	entityManager->FullUpdate();

	/*if (loadFromFile) {
		entityManager->GetCommandProcessor().ReplayFromFile("record.cmd", *entityManager);
	}*/
}

int t = 0;

void GameScene::Update() {

	entityManager->FrameUpdate(camera);

	t++;

	if (t % 60 == 0) {
		entityManager->GetPlayerSystem().AddMinerals(1, 8);
		entityManager->GetPlayerSystem().AddGas(1, 8);
	}

	view->Update(camera);

	camera.Update();


	if (Game::Gamepad.IsButtonReleased(GamepadButton::Select)) {
		entityManager->GetMapSystem().FogOfWarVisible = !entityManager->GetMapSystem().FogOfWarVisible;
	}
}

void GameScene::Draw() {
	const PlayerInfo& playerInfo = entityManager->GetPlayerSystem().GetPlayerInfo(1);

	Platform::DrawOnScreen(ScreenId::Top);

	entityManager->Draw(camera);

	view->DrawUpperScreen(camera);

	Platform::DrawOnScreen(ScreenId::Bottom);

	view->DrawLowerScreen(camera);
}