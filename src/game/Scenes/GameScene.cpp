#include "GameScene.h"

#include "../Game.h"
#include "../GUI/GameHUD.h"
#include "../GUI/Cursor.h"
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

	auto& race = RaceDatabase::Terran;
	race.LoadResourses();


	cursor = new Cursor();
	cursor->Position = { 200,120 };

	camera.Position = { 0,0 };
	camera.Size = { 400,240 };
	camera.Limits = { {0,0,}, size };

	UnitDatabase::LoadAllUnitResources();

	AudioStream* stream = race.GameMusic[std::rand() % race.GameMusic.size()].Stream;
	Game::Audio.PlayStream(stream, 0);

	entityManager = new EntityManager();
	//entityManager->DrawBoundingBoxes = true;
	//entityManager->DrawGrid = true;
	//entityManager->DrawColliders = true;

	entityManager->Init(size);

	hud = new GameHUD(*entityManager, size);

	Color color[] = { Colors::SCRed, Colors::SCBlue, Colors::SCLightGreen, Colors::SCPurle,
	 Colors::SCOrange, Colors::SCGreen, Colors::SCBrown, Colors::SCLightYellow, Colors::SCWhite,
	Colors::SCTeal , Colors::SCYellow , Colors::SCLightBlue };

	int totalPlayers = 8;

	for (int p = 0; p < totalPlayers; ++p) {
		entityManager->GetPlayerSystem().AddPlayer(race, color[p]);
	}

	hud->SetPlayer(1, race);


	//UnitEntityUtil::NewUnit(UnitDatabase::MineralField1, 0,
	//	Vector2Int16(128, 256));

	for (int i = 0; i < 5; ++i) {
		UnitEntityUtil::NewUnit(*UnitDatabase::Units[0], 2,
			Vector2Int16(600, 32 * i + 32));
	}

	//UnitEntityUtil::NewUnit(*UnitDatabase::Units[1], 2,
	//	Vector2Int16(400, 300));

	//EntityId e = UnitEntityUtil::NewUnit(*UnitDatabase::Units[0], 0,
	//	Vector2Int16(48, 48));
	EntityId e = 0;
	int i = 0;
	for (int y = 5; y > 0; --y) {
		for (int x = 5; x > 0; --x) {
			Color c = color[(i) % 12];
			auto& def = *UnitDatabase::Units[0];
			e = UnitEntityUtil::NewUnit(def, 1 + i / 200,// 1 + i % totalPlayers,
				Vector2Int16(Vector2Int{ x * 32 + 48,y * 32 + 48 }));

			//entityManager->UnitArchetype.OrientationComponents.GetComponent(e) = 12;
			//EntityUtil::PlayAnimation(e, def.Graphics->IdleAnimations[0]);
			//EntityUtil::StartTimer(e, 3, TimerExpiredAction::UnitToggleIdleAnimation, true);
			//entityManager->GoTo(e, { 1024,1024 });

			i += 1;

		}
	}


	//entityManager->UnitArchetype.StateComponents.GetComponent(e) = UnitState::Attacking;

	entityManager->FullUpdate(camera);

	hud->context.selectedEntities.AddEntity(e);

	//for (EntityId id : entityManager->UnitArchetype.Archetype.GetEntities()) {
	//	UnitEntityUtil::AttackPosition(id, { 64,64 });
	//}
}

int t = 0;


void GameScene::Update() {

	entityManager->FrameUpdate(camera);

	t++;

	if (t % 60 == 0) {
		entityManager->GetPlayerSystem().AddMinerals(1, 8);
		entityManager->GetPlayerSystem().AddGas(1, 8);
	}

	hud->ApplyInput(camera);

	static std::vector<EntityId> tmp;

	tmp.clear();

	for (EntityId id : hud->context.selectedEntities) {
		if (!entityManager->UnitArchetype.Archetype.HasEntity(id) ||
			entityManager->UnitArchetype.HiddenArchetype.Archetype.HasEntity(id))

			tmp.push_back(id);
	}

	if (tmp.size() > 0)
		hud->context.selectedEntities.RemoveSortedEntities(tmp);

	tmp.clear();

	hud->UpdateSelection();

	bool selectionUpdate = cursor->Update(camera, hud->context);

	// TODO: Player input should be feed in the entity manager and on start of secondary update
	//if (logical)
	{

		if (Game::Gamepad.IsButtonPressed(GamepadButton::Select)) {
			entityManager->GetMapSystem().FogOfWarVisible =
				!entityManager->GetMapSystem().FogOfWarVisible;
		}

		if (Game::Gamepad.IsButtonPressed(GamepadButton::Start)) {
			entityManager->DrawGrid = !entityManager->DrawGrid;
		}


		if (selectionUpdate && hud->context.selectedEntities.size() > 0)
		{
			entityManager->GetSoundSystem().PlayUnitChatSelect(hud->context.selectedEntities[0]);
		}

		if (Game::Gamepad.IsButtonPressed(GamepadButton::X)) {

			Vector2Int16 pos = camera.ScreenToWorld(cursor->Position);

			for (EntityId id : hud->context.selectedEntities)
			{
				UnitEntityUtil::SetAIState(id, UnitAIState::GoToPosition, pos);
				entityManager->GetSoundSystem().PlayUnitChatCommand(id);
			}

			if (hud->context.selectedEntities.size()) {
				hud->NewActionMarker(pos);
			}
		}

		if (Game::Gamepad.IsButtonPressed(GamepadButton::B)) {

			Vector2Int16 pos = camera.ScreenToWorld(cursor->Position);


			EntityId target = entityManager->GetKinematicSystem().PointCast(pos);

			if (target != Entity::None)
			{
				for (EntityId id : hud->context.selectedEntities) {

					if (target == id)
						continue;

					UnitEntityUtil::SetAIState(id, UnitAIState::AttackTarget, target);
					entityManager->GetSoundSystem().PlayUnitChatCommand(id);
				}
			}
			else {
				for (EntityId id : hud->context.selectedEntities) {
					UnitEntityUtil::SetAIState(id, UnitAIState::GoToAttack, pos);
					entityManager->GetSoundSystem().PlayUnitChatCommand(id);
				}
			}

	
			hud->NewActionMarker(pos);
			

		}
		if (Game::Gamepad.IsButtonPressed(GamepadButton::Y)) {

			for (EntityId id : hud->context.selectedEntities)
			{
				UnitEntityUtil::SetAIState(id, UnitAIState::Idle);
				entityManager->GetSoundSystem().PlayUnitChatCommand(id);
			}
		}
	}

	entityManager->GetRenderSystem().SetSelection(hud->context.selectedEntities.GetEntities(), Colors::UIDarkGreen);

	camera.Update();
}

void GameScene::Draw() {
	const PlayerInfo& playerInfo = entityManager->GetPlayerSystem().GetPlayerInfo(1);

	Platform::DrawOnScreen(ScreenId::Top);

	entityManager->Draw(camera);

	hud->UpperScreenGUI(camera);

	cursor->Draw();

	Platform::DrawOnScreen(ScreenId::Bottom);

	hud->LowerScreenGUI(camera);
}