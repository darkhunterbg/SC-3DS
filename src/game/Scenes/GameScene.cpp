#include "GameScene.h"
#include "../Platform.h"
#include "../Map/MapSystem.h"
#include "../Game.h"
#include "../GUI/GameHUD.h"
#include "../GUI/Cursor.h"
#include "../Audio.h"
#include "../Entity/EntityManager.h"
#include "../Profiler.h"
#include "../Data/UnitDatabase.h"


std::array<AudioClip, 2> death;
std::array<AudioClip, 4> what;
std::array<AudioClip, 4> yes;
std::vector<EntityId> selection;


GameScene::GameScene() {

}

GameScene::~GameScene() {
	delete mapSystem;
}

void GameScene::Start() {
	hud = new GameHUD();

	hud->SetMinerals(50);
	hud->SetGas(0);
	hud->SetMaxSupply(10);
	hud->SetUsedSupply(4);

	cursor = new Cursor();
	cursor->Position = { 200,120 };

	MapDef mapDef;
	mapDef.size = { 32,32 };

	mapSystem = new MapSystem(mapDef);
	camera.Position = { 0,0 };
	camera.Size = { 400,240 };
	camera.Limits = mapSystem->GetMapBounds();

	UnitDatabase::LoadAllUnitResources();

	AudioStream* stream = Platform::LoadAudioStream("music/terran1.wav");
	death[0] = Platform::LoadAudioClip("music/tmadth00.wav");
	death[1] = Platform::LoadAudioClip("music/tmadth01.wav");
	what[0] = Platform::LoadAudioClip("music/tmawht00.wav");
	what[1] = Platform::LoadAudioClip("music/tmawht01.wav");
	what[2] = Platform::LoadAudioClip("music/tmawht02.wav");
	what[3] = Platform::LoadAudioClip("music/tmawht03.wav");
	yes[0] = Platform::LoadAudioClip("music/tmayes00.wav");
	yes[1] = Platform::LoadAudioClip("music/tmayes01.wav");
	yes[2] = Platform::LoadAudioClip("music/tmayes02.wav");
	yes[3] = Platform::LoadAudioClip("music/tmayes03.wav");
	Game::Audio->PlayStream(stream, 0);

	//clip.looping = true;
	//clip.AddSprite(marine->GetSprite(13), { 23,19 });
	//clip.AddSprite(marine->GetSprite(30), { 23,19 });
	//clip.AddSprite(marine->GetSprite(47), { 23,19 });
	//clip.AddSprite(marine->GetSprite(30), { 23,19 });
	//clip.frameSize = { 64,64 };
	//for (auto& c : clip.GetFrames())
	//	c.offset -= (clip.frameSize ) / 2;

	entityManager = new EntityManager();


	for (int x = 5; x < 6; ++x) {
		for (int y = 5; y < 6; ++y) {
			entityManager->NewUnit(UnitDatabase::Marine, { x * 32 ,y * 32 });
		}
	}

	selection.push_back(1);
	//int id = 3542;
	//entityManager->DeleteEntity(id);
}

int t = 0;

void GameScene::LogicalUpdate() {
	entityManager->UpdateEntities();

	++t;

	if (t % 60 == 0) {
		hud->AddMinerals(8);
		hud->AddGas(8);
	}
}
std::vector<EntityId> tmp;
void GameScene::Update() {
	frameCounter += 2;

	while (frameCounter >= 5)
	{
		++logicalFrame;
		frameCounter -= 5;
		LogicalUpdate();
	}


	hud->ApplyInput(camera);

	tmp.clear();
	cursor->Update(camera, *entityManager, tmp);
	if (tmp.size() > 0)
	{
		selection.clear();
		selection.insert(selection.begin(), tmp.begin(), tmp.end());

		int i = std::rand() % 4;
		Game::Audio->PlayClip(what[i], 1);
	}

	if (Game::Gamepad.IsButtonPressed(GamepadButton::X)) {

		for (EntityId id : selection)
			if (entityManager->GetRenderComponent(id).depth != -1) {
				auto& nav = entityManager->GetNavigationComponent(id);
				nav.GoTo(camera.ScreenToWorld(cursor->Position));

				int i = std::rand() % 4;
				Game::Audio->PlayClip(yes[i], 1);
			}
	}

	if (Game::Gamepad.IsButtonPressed(GamepadButton::Y)) {

		for (EntityId id : selection)
			if (entityManager->GetRenderComponent(id).depth != -1) {
				auto& nav = entityManager->GetNavigationComponent(id);
				nav.work = false;
			}
	}
	if (Game::Gamepad.IsButtonPressed(GamepadButton::B)) {
		for (EntityId id : selection)
		{
			if (entityManager->GetRenderComponent(id).depth != -1) {
				entityManager->GetAnimationComponent(id).PlayClip(&UnitDatabase::Marine.DeathAnimation);
				entityManager->GetAnimationComponent(id).shadowClip = nullptr;
				entityManager->GetRenderComponent(id).depth = -1;
				entityManager->RemoveColliderComponent(id);
				entityManager->GetNavigationComponent(id).work = false;

				int i = std::rand() % 2;
				Game::Audio->PlayClip(death[i], 1);
			}
		}
	}


	camera.Update();
}

void GameScene::Draw() {
	Platform::DrawOnScreen(ScreenId::Top);

	mapSystem->DrawTiles(camera);

	entityManager->DrawEntites(camera);

	hud->UpperScreenGUI();

	cursor->Draw();

	Platform::DrawOnScreen(ScreenId::Bottom);

	hud->LowerScreenGUI(camera, *mapSystem);

	mapSystem->DrawMiniMapTiles({ 7,108 }, camera);
}