#include "GameScene.h"
#include "../Platform.h"
#include "../Map/MapSystem.h"
#include "../Game.h"
#include "../GUI/GameHUD.h"
#include "../GUI/Cursor.h"
#include "../Audio.h"
#include "../Entity/EntityManager.h"

const SpriteAtlas* marine;



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
	Game::Audio->PlayStream(stream);
	marine = Platform::LoadAtlas("marine.t3x");

	entityManager = new EntityManager();

	for (int x = 0; x < 100; ++x) {
		for (int y = 0; y < 100; ++y) {
			auto e = entityManager->NewEntity({ x*32,y*32 });
			auto c = renderSystem.NewComponent(e);
			entityManager->SetRenderComponent(e, c);
			c->sprite = marine->GetSprite(100);
			
		}
	}

}

int t = 0;

void GameScene::Update() {
	++t;

	if (t % 60 == 0) {
		hud->AddMinerals(8);
		hud->AddGas(8);
	}

	hud->ApplyInput(camera);

	cursor->Update();

	camera.Update();

	
}

void GameScene::Draw() {
	Platform::DrawOnScreen(ScreenId::Top);

	mapSystem->DrawTiles(camera);

	entityManager->UpdateEntities(renderSystem);
	renderSystem.Draw(camera);

	hud->UpperScreenGUI();

	cursor->Draw();

	Platform::DrawOnScreen(ScreenId::Bottom);

	hud->LowerScreenGUI(camera, *mapSystem);

	mapSystem->DrawMiniMapTiles({ 7,108 }, camera);
}