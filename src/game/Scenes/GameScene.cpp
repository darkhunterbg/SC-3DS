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
std::array<AudioClip, 4> what;
std::array<AudioClip, 4> yes;
std::vector<EntityId> selection;

AnimationClip clip;
AnimationClip deathClip;
AnimationClip moveClips[32];

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
	what[0] = Platform::LoadAudioClip("music/tmawht00.wav");
	what[1] = Platform::LoadAudioClip("music/tmawht01.wav");
	what[2] = Platform::LoadAudioClip("music/tmawht02.wav");
	what[3] = Platform::LoadAudioClip("music/tmawht03.wav");
	yes[0] = Platform::LoadAudioClip("music/tmayes00.wav");
	yes[1] = Platform::LoadAudioClip("music/tmayes01.wav");
	yes[2] = Platform::LoadAudioClip("music/tmayes02.wav");
	yes[3] = Platform::LoadAudioClip("music/tmayes03.wav");
	Game::Audio->PlayStream(stream, 0);
	marine = Platform::LoadAtlas("marine.t3x");

	clip.looping = true;
	clip.AddSprite(marine->GetSprite(13), { 23,19 });
	clip.AddSprite(marine->GetSprite(30), { 23,19 });
	clip.AddSprite(marine->GetSprite(47), { 23,19 });
	clip.AddSprite(marine->GetSprite(30), { 23,19 });
	clip.frameSize = { 64,64 };
	for (auto& c : clip.GetFrames())
		c.offset -= (clip.frameSize ) / 2;

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
	{
		c.offset -= (deathClip.frameSize ) / 2;
	}

	entityManager = new EntityManager();


	for (int i = 0; i <= 16; ++i) {
		moveClips[i].frameSize = { 64,64 };
		moveClips[i].looping = true;
		moveClips[i].frameDuration = 1;
		for (int j = 0; j < 9; ++j)
			moveClips[i].AddSprite(marine->GetSprite(i + j * 17 + 68));
	}

	for (int i = 17; i < 32; ++i) {
		moveClips[i].frameSize = { 64,64 };
		moveClips[i].looping = true;
		moveClips[i].frameDuration = 1;
		for (int j = 0; j < 9; ++j)
			moveClips[i].AddSprite(marine->GetSprite(32 - i + j * 17 + 68), { 0,0 }, true);
	}

	for (int i = 0; i < 32; ++i) {
		for (int j = 0; j < moveClips[i].GetFrameCount(); ++j)
		{
			auto& c = moveClips[i];
			auto& f = moveClips[i].GetFrame(j);
			c.SetFrameOffset(j, (c.frameSize - f.sprite.rect.size)/2  - c.frameSize/2);
		}

	}

	for (int x = 5; x < 6; ++x) {
		for (int y = 5; y < 6; ++y) {
			auto e = entityManager->NewEntity({ x * 32 ,y * 32  });
			entityManager->AddRenderComponent(e, clip.GetFrame(0).sprite);
			entityManager->AddColliderComponent(e, { clip.GetFrame(0).offset , clip.GetFrame(0).sprite.rect.size });
			entityManager->AddAnimationComponent(e, &clip);
			auto& nav = entityManager->AddNavigationComponent(e, 1, 4);
			//nav.GoTo({ 256, 256 });

			for (int i = 0; i < 32; ++i) {
				nav.clips[i] = (&moveClips[i]);
			}

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
	bool update = false;
	while (frameCounter >= 5)
	{
		update = true;
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
				entityManager->GetAnimationComponent(id).PlayClip(&deathClip);
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