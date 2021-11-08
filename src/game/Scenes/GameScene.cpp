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

	int totalPlayers = 3;


	const auto& race = *GameDatabase::instance->GetRace(RaceType::Terran);
	if (race.GetMusic())
	{
		_entityManager->SoundSystem.PlayMusic(*race.GetMusic());
	}
	//	entityManager->GetSoundSystem().PlayMusic(*race.GetMusic());


	for (int p = 0; p < totalPlayers; ++p)
	{
		auto& r = *GameDatabase::instance->GetRace((RaceType)(p % 3 + 1));
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

	const UnitDef* units[3];
	units[0] = GameDatabase::instance->GetUnit("Terran\\Units\\Marine");
	units[1] = GameDatabase::instance->GetUnit("Protoss\\Units\\Zealot");
	units[2] = GameDatabase::instance->GetUnit("Zerg\\Units\\Zergling");

	std::vector<const UnitDef*> group0 = { units[0], units[0], units[0] };
	std::vector<const UnitDef*> group2= { units[1] };
	std::vector<const UnitDef*> group1 = { units[2],units[2],units[2],units[2] };

	std::vector<const  UnitDef*>* groups[] = { &group0, &group1, &group2 };

	int i = 0;
	for (int y = 1; y < 20; ++y)
	{
		for (int x = 1; x < 20; ++x)
		{

			auto& group = *groups[i % 3];

			for (int j = 0; j < group.size(); ++j)
			{
				EntityUtil::SpawnUnit(*group[j], PlayerId{ (short)(i % totalPlayers + 1) },
					Vector2Int16(Vector2Int{ x * 70 + (j%2)*32 ,y * 70 + (j/2) *32}));
			}
			//EntityUtil::SpawnUnit(*units[i % 3], PlayerId{ (short)(i % totalPlayers + 1) }, Vector2Int16(Vector2Int{ x * 64 ,y * 64 }));
			++i;
		}
	}

	_entityManager->PlayerSystem.SetMapKnown(PlayerId{ 1 });

	//_entityManager->SetOrientation(0, 16);
	//_entityManager->UnitSystem.GetAIComponent(0).targetPosition = Vector2Int16(256, 256);
	//EntityUtil::SetUnitAIState(0, UnitAIStateId::Walk);

	//EntityUtil::SpawnUnit(def, PlayerId{ 1 }, Vector2Int16(Vector2Int{ 64 ,128 }));

	//EntityUtil::SpawnUnit(def, PlayerId{ 1 }, Vector2Int16(Vector2Int{ 96 , 64 }));
	//EntityUtil::SpawnUnit(def, PlayerId{ 1 }, Vector2Int16(Vector2Int{ 100 , 80 }));
	//EntityUtil::SpawnUnit(def, PlayerId{ 1 }, Vector2Int16(Vector2Int{ 110 , 96 }));
	//EntityUtil::SpawnUnit(def, PlayerId{ 1 }, Vector2Int16(Vector2Int{ 100 , 48 }));

	//EntityUtil::SpawnUnit(*GameDatabase::instance->GetUnit("Protoss\\Units\\Zealot"), PlayerId{ 3 }, Vector2Int16(Vector2Int{ 256 , 128 }));
	//EntityUtil::SpawnUnit(*GameDatabase::instance->GetUnit("Protoss\\Units\\Zealot"), PlayerId{ 3 }, Vector2Int16(Vector2Int{ 128 , 96 }));

	//EntityUtil::SpawnUnit(*GameDatabase::instance->GetUnit("Zerg\\Units\\Zergling"), PlayerId{ 2 }, Vector2Int16(Vector2Int{ 300 , 300 }));
	//EntityUtil::SpawnUnit(*GameDatabase::instance->GetUnit("Zerg\\Units\\Zergling"), PlayerId{ 2 }, Vector2Int16(Vector2Int{ 260 , 300 }));
	//EntityUtil::SpawnUnit(*GameDatabase::instance->GetUnit("Zerg\\Units\\Zergling"), PlayerId{ 2 }, Vector2Int16(Vector2Int{ 260 , 260 }));

	_updateCrt = _entityManager->NewUpdateCoroutine();

	_updateCrt->RunAll();

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

void GameScene::Frame(TimeSlice& frameBudget)
{
	GraphicsRenderer::DrawOnScreen(ScreenId::Top);

	_entityManager->Draw(_view->GetCamera());

	_view->Draw();

	frameTime -= std::min(0.0166f, Game::DeltaTime);

	if (frameTime < 0)
	{
		_updateCrt->RunAll();
		_updateCrt->Restart();
		_entityManager->UpdateAudio(_view->GetCamera());

		frameTime += 1.0 / (24.0 * GameSpeed);
	}

	while (!frameBudget.IsRemainingLessThan(0.002) && !_updateCrt->IsCompleted())
	{
		_updateCrt->Next();
	}

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
