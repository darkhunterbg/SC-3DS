#include "PerformanceTestScene.h"
#include "Data/UnitDatabase.h"
#include "../Platform.h"
#include "../Profiler.h"
#include <algorithm>
#include "../Job.h"
#include "../Entity/Component.h"

void PerformanceTestScene::Start() {

	UnitDatabase::LoadAllUnitResources();

	camera.Position = { 400,240 };
	camera.Size = { 400,240 };
	camera.Scale = 1;

	entityManager.Init({ 128 * 32,128 * 32 });
	//entityManager.DrawColliders = true;

	for (int i = 0; i < 10000; ++i) {
		entityManager.NewUnit(UnitDatabase::Marine,
			Vector2Int16(Vector2Int{ (i / 100) * 32 + 16, (i % 100) * 32 + 16 }),
			Colors::Red);

		//entityManager.RenderArchetype.Archetype.RemoveEntity(i);
		entityManager.MovementArchetype.MovementComponents[i].velocity = 1;
	}
}
static int t = 0;
static int c = 0;

void PerformanceTestScene::Update() {
	entityManager.UpdateSecondaryEntities();
	entityManager.UpdateEntities();
}



void PerformanceTestScene::Draw() {
	Platform::DrawOnScreen(ScreenId::Top);
	entityManager.DrawEntites(camera);
}