#include "GameScene.h"

#include "../Game.h"
#include "../SceneView/GameSceneView.h"
#include "../Engine/AudioManager.h"
#include "../Entity/EntityManager.h"
#include "../Profiler.h"
#include "../Data//GameDatabase.h"

#include "../Engine/GraphicsRenderer.h"
#include "../Entity/EntityUtil.h"
#include "../Engine/AssetLoader.h"
#include "../TimeSlice.h"

#include <algorithm>

GameScene::GameScene() { _id = NAMEOF(GameScene); }

GameScene::~GameScene() {}


void GameScene::Start()
{
	Vector2Int16 size = { 64 * 32,64 * 32 };


	_entityManager = new EntityManager();
	//entityManager->DrawBoundingBoxes = true;
	//entityManager->DrawGrid = true;
	//entityManager->DrawColliders = true;

	_entityManager->Init(size);


	//view = new GameView(*entityManager, size);

	Color color[] = { Colors::SCRed, Colors::SCBlue, Colors::SCLightGreen, Colors::SCPurle,
	 Colors::SCOrange, Colors::SCGreen, Colors::SCBrown, Colors::SCLightYellow, Colors::SCWhite,
	Colors::SCTeal , Colors::SCYellow , Colors::SCLightBlue };

	int totalPlayers = 8;


	const auto& race = *GameDatabase::instance->GetRace(RaceType::Terran);
	if (race.GetMusic())
	{
		_entityManager->SoundSystem.PlayMusic(*race.GetMusic());
	}
	//	entityManager->GetSoundSystem().PlayMusic(*race.GetMusic());


	for (int p = 0; p < totalPlayers; ++p)
	{
		auto& r = *GameDatabase::instance->GetRace((RaceType)(p % 3 + 1) );
		_entityManager->PlayerSystem.AddPlayer(r, color[p]);
	}

	_view = new GameSceneView(this);
	_view->SetPlayer(PlayerId{ 1 });

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
		for (int x = 1; x < 2; ++x)
		{
			EntityUtil::SpawnUnit(def, PlayerId{ 1 }, Vector2Int16(Vector2Int{ x * 32 ,y * 32 }));
		}
	}
	_entityManager->PlayerSystem.SetMapKnown(PlayerId{ 1 });

	EntityUtil::SpawnUnit(def, PlayerId{ 2 }, Vector2Int16(Vector2Int{ 128 , 64 }));

	//EntityUtil::SpawnUnit(def, PlayerId{ 3 }, Vector2Int16(Vector2Int{ 300 , 300 }));

	_updateCrt = _entityManager->NewUpdateCoroutine();

	_updateCrt->RunAll();

	_updateCrt->Restart();


	/*if (loadFromFile) {
		entityManager->GetCommandProcessor().ReplayFromFile("record.cmd", *entityManager);
	}*/
}

void GameScene::Stop()
{
	delete _view;
	_view = nullptr;

	AssetLoader::GetDatabase().ReleaseLoadedAssets();

}


static double frameTime = 1.0 / 24.0;

void GameScene::Frame()
{
	TimeSlice ts(0.015);

	GraphicsRenderer::DrawOnScreen(ScreenId::Top);

	_entityManager->Draw(_view->GetCamera());

	_view->Draw();



	frameTime -= 1.0 / 60.0;

	if (frameTime < 0)
	{
		_updateCrt->RunAll();
		_updateCrt->Restart();

		frameTime += 1.0 / (24.0 * GameSpeed);
	}

	do
	{
		_updateCrt->Next();
	} while (!ts.IsTimeElapsed() && !_updateCrt->IsCompleted());

	_view->Update();
	
	if (Game::GetInput().Cheats.ToggleFoW.IsActivated())
	{
		_entityManager->MapSystem.FogOfWarVisible = !_entityManager->MapSystem.FogOfWarVisible;
	}

}

void GameScene::OnPlatformChanged()
{
	if (_view)
		_view->OnPlatformChanged();
}
