#include "PerformanceTestScene.h"
#include "Data/UnitDatabase.h"
#include "../Platform.h"
#include "../Profiler.h"
#include <algorithm>
#include "../Job.h"
#include "../Entity/Component.h"



void PerformanceTestScene::Start() {

	camera.Position = { 400,240 };
	camera.Size = { 400,240 };
	camera.Scale = 2;

	for (int i = 0; i < 10000; ++i) {
		entityManager.NewUnit(UnitDatabase::Marine,
			Vector2Int16(Vector2Int{ (i / 100) * 32 + 16, (i % 100) * 32 + 16 }),
			Colors::Red);
	}

	entityManager.UpdateEntities();
}
static int t = 0;
static int c = 0;

void PerformanceTestScene::Update() {

	c = 0;
	for (int i = 0; i < 10000; ++i) {
		entityManager.SetPosition(i, entityManager.PositionComponents[i] + Vector2Int16{1, 1});
	}
	entityManager.UpdateEntities();

	/*SectionProfiler p("CollidesWith");

	for (int i = 0; i < 1000; ++i) {
		c += entityManager.CollidesWithAny(entityManager.CollisionArchetype.ColliderComponents[i].worldCollider, i) ?
			1 : 0;

	}
	p.Submit();*/
}



void PerformanceTestScene::Draw() {
	Platform::DrawOnScreen(ScreenId::Top);

	entityManager.DrawEntites(camera);

}