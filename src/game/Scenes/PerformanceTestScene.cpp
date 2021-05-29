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

	camera.Position = { 400 ,240 };
	camera.Size = { 400,240 };
	camera.Scale = 2;

	entityManager.Init({ 128 * 32,128 * 32 });
	entityManager.GetPlayerSystem().AddPlayer(RaceDatabase::Terran, Colors::White);
	entityManager.GetMapSystem().FogOfWarVisible = false;

	for (int i = 0; i < 10000; ++i) {
		EntityId e = entityManager.NewEntity();
		entityManager.UnitArchetype.RenderArchetype.Archetype.AddEntity(e);
		EntityUtil::SetRenderFromAnimationClip(e, UnitDatabase::Marine.Graphics->IdleAnimations[0], 0);
		EntityUtil::SetPosition(e, Vector2Int16((i % 100) * 32 + 16, i * 32 / 100 + 16));
		entityManager.UnitArchetype.RenderArchetype.RenderComponents.GetComponent(e).unitColor =
			Color32(Colors::White);
	}

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

	entityManager.FullUpdate();
}
static int t = 0;
static int c = 0;


static std::vector<EntityId> v;

void PerformanceTestScene::Update() {

	//v.clear();

	//entityManager.NewEntities(1000, v);

	//for (int i = 0; i < v.size(); ++i) {
	//	UnitEntityUtil::NewUnit(UnitDatabase::Marine, 0, { 0,0 });
	//}

	//{
	//	SectionProfiler p("Delete");

	//	entityManager.ClearEntitiesArchetypes(v);
	//}
	//entityManager.ClearEntities();
	entityManager.FrameUpdate(camera);
}



void PerformanceTestScene::Draw() {
	Platform::DrawOnScreen(ScreenId::Top);
	entityManager.Draw(camera);
}