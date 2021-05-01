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
	clip.AddSprite(marine->GetSprite(13), { 23,19 });
	clip.AddSprite(marine->GetSprite(30), { 23,19 });
	clip.AddSprite(marine->GetSprite(47), { 23,19 });
	clip.AddSprite(marine->GetSprite(30), { 23,19 });
	clip.frameSize = { 64,64 };
	for (auto& c : clip.GetFrames())
		c.offset -= clip.frameSize;

	deathClip.AddSprite(marine->GetSprite(221), { 22,19 });
	deathClip.AddSprite(marine->GetSprite(222), { 23,19 });
	deathClip.AddSprite(marine->GetSprite(223), { 17,17 });
	deathClip.AddSprite(marine->GetSprite(224), { 12,14 });
	deathClip.AddSprite(marine->GetSprite(225), { 7,15 });
	deathClip.AddSprite(marine->GetSprite(226), { 2,17 });
	deathClip.AddSprite(marine->GetSprite(227), { 0,19 });
	deathClip.AddSprite(marine->GetSprite(228), { 0,20 });
	deathClip.frameSize = { 64,64 };
	for (auto& c : deathClip.GetFrames())
		c.offset -= deathClip.frameSize;

	entityManager = new EntityManager();

	for (int x = 50 - 1; x >= 0; --x) {
		for (int y = 100 - 1; y >= 0; --y) {
			auto e = entityManager->NewEntity({ x * 32 + 16,y * 32 + 16 });
			entityManager->AddRenderComponent(e, clip.GetFrame(0).sprite);
			entityManager->AddColliderComponent(e, { clip.GetFrame(0).offset , clip.GetFrame(0).sprite.rect.size });
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

	auto p = camera.ScreenToWorld(cursor->Position);

	SectionProfiler profiler("Pointcast");

	EntityId hover = entityManager->PointCast(p);

	profiler.Submit();

	cursor->isHoverState = hover != Entity::None;

	if (Game::Gamepad.A && cursor->isHoverState) {

		entityManager->GetAnimationComponent(hover).PlayClip(&deathClip);
		entityManager->GetRenderComponent(hover).depth = -1;
		entityManager->RemoveColliderComponent(hover);

		int i = std::rand() % 2;
		Game::Audio->PlayClip(death[i], 1);

	}

	hud->ApplyInput(camera);

	cursor->Update(camera);

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