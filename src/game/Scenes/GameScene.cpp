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


	for (int p = 0; p < totalPlayers; ++p)
	{
		entityManager->PlayerSystem.AddPlayer(race, color[p]);
	}

	view.Init();

	view.SetPlayer(PlayerId{ 1 });

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
	for (int y = 1; y < 3; ++y)
	{
		for (int x = 1; x < 3; ++x)
		{
			EntityUtil::SpawnUnit(def, PlayerId{ 1 }, Vector2Int16(Vector2Int{ x * 32 ,y * 32 }));
		}
	}
	entityManager->PlayerSystem.SetMapKnown(PlayerId{ 1 });

	EntityUtil::SpawnUnit(def, PlayerId{ 2 }, Vector2Int16(Vector2Int{ 256 , 64 }));

	entityManager->FullUpdate();

	/*if (loadFromFile) {
		entityManager->GetCommandProcessor().ReplayFromFile("record.cmd", *entityManager);
	}*/
}

int t = 0;

void GameScene::Update()
{

	entityManager->FrameUpdate(view.GetCamera());


	view.GetCamera().Update();


	if (InputManager::Gamepad.IsButtonReleased(GamepadButton::Select))
	{
		entityManager->MapSystem.FogOfWarVisible = !entityManager->MapSystem.FogOfWarVisible;
	}
}

void GameScene::Draw()
{
	GraphicsRenderer::DrawOnScreen(ScreenId::Top);

	entityManager->Draw(view.GetCamera());

	view.DrawMainScreen();

	GraphicsRenderer::DrawOnScreen(ScreenId::Bottom);

	view.DrawSecondaryScreen();
}