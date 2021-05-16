#include "GameScene.h"
//#include "../Platform.h"
#include "../Game.h"
#include "../GUI/GameHUD.h"
#include "../GUI/Cursor.h"
#include "../Audio.h"
#include "../Entity/EntityManager.h"
#include "../Profiler.h"
#include "../Data/UnitDatabase.h"
#include "../Data/RaceDatabase.h"
#include "../Entity/EntityUtil.h"

static std::vector<EntityId> selection;

GameScene::GameScene() {

}

GameScene::~GameScene() {

}

void GameScene::Start() {
	Vector2Int16 size = { 64 * 32,64 * 32 };

	auto& race = RaceDatabase::Terran;
	race.LoadResourses();

	hud = new GameHUD(race, size);


	cursor = new Cursor();
	cursor->Position = { 200,120 };



	camera.Position = { 0,0 };
	camera.Size = { 400,240 };
	camera.Limits = { {0,0,}, size };

	UnitDatabase::LoadAllUnitResources();


	AudioStream* stream = race.GameMusic[std::rand() % race.GameMusic.size()].Stream;
	Game::Audio.PlayStream(stream, 0);

	entityManager = new EntityManager();
	//entityManager->DrawColliders = true;

	entityManager->Init(size);

	entityManager->GetPlayerSystem().AddPlayer(race, Colors::SCBlue);

	Color color[] = { Colors::SCRed, Colors::SCBlue, Colors::SCLightGreen, Colors::SCPurle,
	 Colors::SCOrange, Colors::SCGreen, Colors::SCBrown, Colors::SCLightYellow, Colors::SCWhite,
	Colors::SCTeal , Colors::SCYellow , Colors::SCLightBlue };

	int i = 0;
	for (int y = 40; y >= 0; --y) {
		for (int x = 40; x >= 0; --x) {

			
			Color c = color[(i) % 12];
			auto& def = *UnitDatabase::Units[(i) % UnitDatabase::Units.size()];
			EntityId e = UnitEntityUtil::NewUnit(def, 0,
				Vector2Int16(Vector2Int{ x * 32 + 16,y * 32 + 16 }));

			//entityManager->UnitArchetype.OrientationComponents.GetComponent(e) = 12;
			EntityUtil::PlayAnimation(e, def.Graphics->AttackAnimations[12]);
			//EntityUtil::StartTimer(e, 3, TimerExpiredAction::UnitToggleIdleAnimation, true);
			//entityManager->GoTo(e, { 256,128 });
			
			i += 1;

		}
	}

	selection.push_back(0);
}

int t = 0;

void GameScene::AuxilaryUpdate() {

	SectionProfiler p("AuxUpdate");

	entityManager->UpdateSecondaryEntities();

	p.Submit();
}


void GameScene::LogicalUpdate() {
	SectionProfiler p("LogUpdate");

	entityManager->UpdateEntities();

	++t;

	if (t % 60 == 0) {
		entityManager->GetPlayerSystem().AddMinerals(0,8);
		entityManager->GetPlayerSystem().AddGas(0, 8);
	}

	p.Submit();
}
std::vector<EntityId> tmp;
void GameScene::Update() {
	frameCounter += 2;
	frameCounter2 += 2;

	bool logical = false;

	if (frameCounter2 >= 5)
	{

		frameCounter2 -= 5;
		AuxilaryUpdate();
	}

	if (frameCounter >= 5)
	{
		logical = true;

		++logicalFrame;
		frameCounter -= 5;
		LogicalUpdate();
	}


	hud->ApplyInput(camera);

	tmp.clear();
	cursor->Update(camera, *entityManager, tmp);

	// TODO: Player input should be feed in the entity manager and on start of secondary update
	//if (logical)
	{

		if (tmp.size() > 0)
		{
			selection.clear();
			selection.insert(selection.begin(), tmp.begin(), tmp.end());

			auto& def = entityManager->UnitArchetype.UnitComponents[selection[0]].def;
			int i = std::rand() % def->Sounds.What.TotalClips;

			Game::Audio.PlayClip(def->Sounds.What.Clips[i], 1);
		}

		if (Game::Gamepad.IsButtonPressed(GamepadButton::X)) {

			for (EntityId id : selection)
				if (entityManager->UnitArchetype.Archetype.HasEntity(id)) {
					entityManager->GoTo(id, Vector2Int16(camera.ScreenToWorld(cursor->Position)));

					auto& def = entityManager->UnitArchetype.UnitComponents[id].def;

					int i = std::rand() % def->Sounds.Yes.TotalClips;
					Game::Audio.PlayClip(def->Sounds.Yes.Clips[i], 1);
				}
		}

		if (Game::Gamepad.IsButtonPressed(GamepadButton::Y)) {

			for (EntityId id : selection)
				if (entityManager->NavigationArchetype.Archetype.HasEntity(id)) {
					entityManager->FlagComponents.GetComponent(id).clear(ComponentFlags::NavigationWork);
				}
		}
		if (Game::Gamepad.IsButtonPressed(GamepadButton::B)) {
			entityManager->UnitArchetype.Archetype.RemoveEntities(selection, false);
			entityManager->CollisionArchetype.Archetype.RemoveEntities(selection, false);
			entityManager->NavigationArchetype.Archetype.RemoveEntities(selection, false);
			entityManager->MovementArchetype.Archetype.RemoveEntities(selection, false);


			for (EntityId id : selection)
			{
				auto& unit = entityManager->UnitArchetype.UnitComponents[id];

				auto& def = entityManager->UnitArchetype.UnitComponents[id].def;

				if (unit.HasMovementGlow()) {
					entityManager->DeleteEntity(unit.movementGlowEntity);
					entityManager->ParentArchetype.Archetype.RemoveEntity(id);
				}

				EntityUtil::PlayAnimation(id, def->Graphics->DeathAnimation);

				TimerExpiredAction action = def->Graphics->HasRemnants() ?
					TimerExpiredAction::UnitRemnantsThenDelete : TimerExpiredAction::DeleteEntity;

				EntityUtil::StartTimer(id, def->Graphics->DeathAnimation.GetDuration(), action);

				int i = std::rand() % def->Sounds.Death.TotalClips;
				Game::Audio.PlayClip(def->Sounds.Death.Clips[i], 1);

			}

			selection.clear();
		}
	}

	camera.Update();
}

void GameScene::Draw() {
	const PlayerInfo& playerInfo = entityManager->GetPlayerSystem().GetPlayerInfo(0);

	hud->UpdateInfo(playerInfo);
	
	Platform::DrawOnScreen(ScreenId::Top);

	entityManager->DrawEntites(camera);

	hud->UpperScreenGUI();

	cursor->Draw();

	Platform::DrawOnScreen(ScreenId::Bottom);

	hud->LowerScreenGUI(camera, entityManager->GetMapSystem());
}