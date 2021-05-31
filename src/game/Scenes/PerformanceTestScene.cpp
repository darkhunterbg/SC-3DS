#include "PerformanceTestScene.h"
#include "../Data/UnitDatabase.h"
#include "../Data/RaceDatabase.h"
#include "../Platform.h"
#include "../Profiler.h"
#include <algorithm>
#include "../Job.h"
#include "../Entity/Component.h"
#include "../Entity/EntityUtil.h"
#include "../Engine/GraphicsRenderer.h"


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
		Vector2Int16 pos = Vector2Int16((i % 100) * 32 + 16, (i  / 100)*32 + 16);
		EntityUtil::SetPosition(e,pos);
		entityManager.UnitArchetype.RenderArchetype.RenderComponents.GetComponent(e).unitColor =
			Color32(Colors::White);
	}

	entityManager.FullUpdate();
}
static int t = 0;
static int c = 0;


static std::vector<EntityId> v;

void PerformanceTestScene::Update() {

	entityManager.FrameUpdate(camera);
}

void PerformanceTestScene::Draw() {
	GraphicsRenderer::DrawOnScreen(ScreenId::Top);
	entityManager.Draw(camera);
}