#include "PerformanceTestScene.h"
#include "../Data/UnitDatabase.h"
#include "../Data/RaceDatabase.h"
#include "../Platform.h"
#include "../Profiler.h"
#include <algorithm>
#include "../Job.h"
#include "../Entity/Component.h"
#include "../Entity/EntityUtil.h"


void PerformanceTestScene::Start() {

	UnitDatabase::LoadAllUnitResources();
	RaceDatabase::Terran.LoadResourses();

	camera.Position = { 200 ,120 };
	camera.Size = { 400,240 };
	camera.Scale = 1;

	entityManager.Init({ 128 * 32,128 * 32 });


	//entityManager.DrawColliders = true;

	//for (int i = 0; i < 5000; ++i) {
	//	entityManager.NewUnit(UnitDatabase::SCV,
	//		Vector2Int16(Vector2Int{ (i / 100) * 32 + 16, (i % 100) * 32 + 16 }),
	//		Colors::Red);

	//	//if (i % 4 == 0)
	//	EntityUtil::StartTimer(i, TimeUtil::SecondsTime(1), TimerExpiredAction::DeleteEntity);


	//	//entityManager.PlayUnitAnimation(i, UnitDatabase::Marine.MovementAnimations[4]);

	//	//entityManager.CollisionArchetype.Archetype.RemoveEntity(i);
	//	//entityManager.NavigationArchetype.Archetype.RemoveEntity(i);
	//	//entityManager.MovementArchetype.MovementComponents[i].velocity = { 2,2 };
	//	//entityManager.UnitArchetype.OrientationComponents[i].changed = true;
	//	//entityManager.UnitArchetype.OrientationComponents[i].orientation = 12;

	//	//entityManager.GoTo(i, { 1024,1024 });
	//}
}
static int t = 0;
static int c = 0;

static unsigned frameCounter = 2;
static unsigned frameCounter2 = 0;


static std::vector<EntityId> v;

void PerformanceTestScene::Update() {

	v.clear();

	entityManager.NewEntities(1000, v);

	for (int i = 0; i < v.size(); ++i) {
		entityManager.NewUnit(UnitDatabase::Marine, { 0,0 }, Colors::White, v[i]);
	}

	{
		SectionProfiler p("Delete");

		entityManager.ClearEntitiesArchetypes(v);
	}
	entityManager.ClearEntities();
	frameCounter += 2;
	frameCounter2 += 2;


	while (frameCounter2 >= 5)
	{
		frameCounter2 -= 5;
		entityManager.UpdateSecondaryEntities();
	}

	while (frameCounter >= 5)
	{
		frameCounter -= 5;
		entityManager.UpdateEntities();
	}
}



void PerformanceTestScene::Draw() {
	Platform::DrawOnScreen(ScreenId::Top);
	entityManager.DrawEntites(camera);
}