#include "GameScene.h"
//#include "../Platform.h"
#include "../Map/MapSystem.h"
#include "../Game.h"
#include "../GUI/GameHUD.h"
#include "../GUI/Cursor.h"
#include "../Audio.h"
#include "../Entity/EntityManager.h"
#include "../Profiler.h"
#include "../Data/UnitDatabase.h"


static std::vector<EntityId> selection;


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
	mapDef.size = { 128,128 };

	mapSystem = new MapSystem(mapDef);
	camera.Position = { 0,0 };
	camera.Size = { 400,240 };
	camera.Limits = mapSystem->GetMapBounds();

	UnitDatabase::LoadAllUnitResources();

	// TODO music is in multiple folders
	AudioStream* stream = Game::AssetLoader.LoadAudioStream("music/terran1.wav");
	Game::Audio.PlayStream(stream, 0);

	entityManager = new EntityManager();
	//entityManager->DrawColliders = true;

	entityManager->Init(Vector2Int16(mapSystem->GetMapBounds().size));

	Color color[] = { Colors::SCRed, Colors::SCBlue, Colors::SCLightGreen, Colors::SCPurle,
	 Colors::SCOrange, Colors::SCGreen, Colors::SCBrown, Colors::SCLightYellow, Colors::SCWhite,
	Colors::SCTeal , Colors::SCYellow , Colors::SCLightBlue };

	int i = 0;
	for (int y = 2; y >= 0; --y) {
		for (int x = 2; x >= 0; --x) {

			Color c = color[(i) % 12];
			EntityId e = entityManager->NewUnit(*UnitDatabase::Units[(i) % UnitDatabase::Units.size()],
				Vector2Int16(Vector2Int{ x * 64 + 16,y * 64 + 16 }), c);
			
			//entityManager->UnitArchetype.OrientationComponents[i].orientation = 24;
			//entityManager->UnitArchetype.OrientationComponents[i].changed = true;
			//entityManager->CollisionArchetype.Archetype.RemoveEntity(e);
			//int orientation = std::rand() % 32;
			i++;

			/*if (i % 2) {
				entityManager->RenderArchetype.Archetype.RemoveEntity(e);
				entityManager->AnimationArchetype.Archetype.RemoveEntity(e);
			}*/
			//entityManager->SetOrientation(e, orientation);
			//entityManager->GoTo(e, Vector2Int16(Vector2Int{ 512 * ((i +1)% 2) +32, 512 * (i % 2) } +32));
		}
	}

	selection.push_back(0);
	//int id = 3542;
	//entityManager->DeleteEntity(id);
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
		hud->AddMinerals(8);
		hud->AddGas(8);
	}

	p.Submit();
}
std::vector<EntityId> tmp;
void GameScene::Update() {
	frameCounter += 2;
	frameCounter2 += 2;

	while (frameCounter2 >= 5)
	{
		frameCounter2 -= 5;
		AuxilaryUpdate();
	}

	while (frameCounter >= 5)
	{

		++logicalFrame;
		frameCounter -= 5;
		LogicalUpdate();
	}

	//int i = 0;
	//for (int y = 49; y >= 0; --y) {
	//	for (int x = 99; x >= 0; --x) {

	//		entityManager->SetPosition(i, Vector2Int16(Vector2Int{ x * 32 + 16 ,y * 32 + 16 }));

	//		i++;
	//	}
	//}


	hud->ApplyInput(camera);

	tmp.clear();
	cursor->Update(camera, *entityManager, tmp);
	if (tmp.size() > 0)
	{
		selection.clear();
		selection.insert(selection.begin(), tmp.begin(), tmp.end());

		int i = std::rand() % 4;
		auto& def = entityManager->UnitArchetype.UnitComponents[selection[0]].def;
		Game::Audio.PlayClip(def->SelectedSoundDef.Clips[i], 1);
	}

	if (Game::Gamepad.IsButtonPressed(GamepadButton::X)) {

		for (EntityId id : selection)
			if (entityManager->RenderArchetype.RenderComponents.GetComponent(id).depth != -1) {
				entityManager->GoTo(id, Vector2Int16(camera.ScreenToWorld(cursor->Position)));

				auto& def = entityManager->UnitArchetype.UnitComponents[id].def;

				int i = std::rand() % def->ActionConfirmSoundDef.TotalClips;
				Game::Audio.PlayClip(def->ActionConfirmSoundDef.Clips[i], 1);
			}
	}

	if (Game::Gamepad.IsButtonPressed(GamepadButton::Y)) {

		for (EntityId id : selection)
			if (entityManager->RenderArchetype.RenderComponents.GetComponent(id).depth != -1) {
				entityManager->FlagComponents.GetComponent(id).clear(ComponentFlags::NavigationWork);
			}
	}
	if (Game::Gamepad.IsButtonPressed(GamepadButton::B)) {
		for (EntityId id : selection)
		{
			if (entityManager->RenderArchetype.RenderComponents[id].depth != -1) {
				auto& def = entityManager->UnitArchetype.UnitComponents[id].def;

				entityManager->PlayUnitAnimation(id, def->DeathAnimation);
				entityManager->RenderArchetype.RenderComponents[id].depth = -1;
				entityManager->CollisionArchetype.Archetype.RemoveEntity(id);
				entityManager->NavigationArchetype.Archetype.RemoveEntity(id);
				entityManager->MovementArchetype.Archetype.RemoveEntity(id);

			

				int i = std::rand() % def->DeathSoundDef.TotalClips;
				Game::Audio.PlayClip(def->DeathSoundDef.Clips[i], 1);
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