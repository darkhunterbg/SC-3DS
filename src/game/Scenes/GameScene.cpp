#include "GameScene.h"
#include "../Platform.h"
#include "../Map/MapSystem.h"
#include "../Game.h"
#include "../GUI/GameHUD.h"
#include "../GUI/Cursor.h"
#include "../Audio.h"
#include "../Entity/EntityManager.h"
#include "../Profiler.h"

const SpriteAtlas* marine;

std::array<AudioClip, 2> death;

AnimationClip clip;
AnimationClip deathClip;

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

	AudioStream* stream = Platform::LoadAudioStream("music/terran1.wav");
	death[0] = Platform::LoadAudioClip("music/tmadth00.wav");
	death[1] = Platform::LoadAudioClip("music/tmadth01.wav");
	Game::Audio->PlayStream(stream, 0);
	marine = Platform::LoadAtlas("marine.t3x");

	clip.looping = true;
	clip.AddSprite(marine->GetSprite(13));
	clip.AddSprite(marine->GetSprite(30));
	clip.AddSprite(marine->GetSprite(47));
	clip.AddSprite(marine->GetSprite(30));

	deathClip.AddSprite(marine->GetSprite(221));
	deathClip.AddSprite(marine->GetSprite(222));
	deathClip.AddSprite(marine->GetSprite(223));
	deathClip.AddSprite(marine->GetSprite(224));
	deathClip.AddSprite(marine->GetSprite(225));
	deathClip.AddSprite(marine->GetSprite(226));
	deathClip.AddSprite(marine->GetSprite(227));
	deathClip.AddSprite(marine->GetSprite(228));

	entityManager = new EntityManager();

	for (int x = 0; x < 50; ++x) {
		for (int y = 0; y < 100; ++y) {
			auto e = entityManager->NewEntity({ x * 32,y * 32 });
			entityManager->AddRenderComponent(e, clip.GetSprites()[0]);
			entityManager->AddAnimationComponent(e, &clip);
		}
	}

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

void GameScene::Update() {
	frameCounter += 2;
	bool update = false;
	while (frameCounter >= 5)
	{
		update = true;
		++logicalFrame;
		frameCounter -= 5;
		LogicalUpdate();
	}

	if (Game::Gamepad.A) {
		auto p = camera.ScreenToWorld(cursor->Position);

		for (int x = 0; x < 50; ++x) {
			for (int y = 0; y < 100; ++y) {
				Rectangle rect = { {x * 32,y * 32},{25,25} };

				if (rect.Contains(p)) {
					int id = x * 100 + y + 1;
					if (entityManager->HasEntity(id)) {
						//entityManager->DeleteEntity(id);
						auto* cmp = entityManager->GetAnimationComponent(id);
						if (cmp->clip == &deathClip)
							continue;
						entityManager->GetAnimationComponent(id)->PlayClip(&deathClip);

						int i = std::rand() % 2;
						Game::Audio->PlayClip(death[i], 1);
						
					}
				}
			}
		}
	}

	hud->ApplyInput(camera);

	cursor->Update();

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