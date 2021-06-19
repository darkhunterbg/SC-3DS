#include "PerformanceTestScene.h"
#include "../Data/RaceDatabase.h"
#include "../Profiler.h"

#include "../Entity/Component.h"
#include "../Entity/EntityUtil.h"
#include "../Engine/GraphicsRenderer.h"

#include "../Data/GameDatabase.h"

#include <algorithm>

void PerformanceTestScene::Start() {

	RaceDatabase::Terran.LoadResourses();

	camera.Position = { 400 ,240 };
	camera.Size = { 400,240 };
	camera.Scale = 2;

	em.Init({ 128 * 32,128 * 32 });
	em.GetPlayerSystem().AddPlayer(RaceDatabase::Terran, Colors::White);
	em.GetMapSystem().FogOfWarVisible = false;
	//em.DrawBoundingBoxes = true;

	const auto& def = *GameDatabase::instance->GetUnit("Terran\\Units\\Marine");
	const auto& clip = GameDatabase::instance->AnimClipDefs[0];

	for (int i = 0; i < 10000; ++i) {
		EntityId e = em.NewEntity();
		em.RenderArchetype.Archetype.AddEntity(e);
		em.AnimationArchetype.Archetype.AddEntity(e);
		em.FlagComponents.GetComponent(e).set(ComponentFlags::RenderEnabled);
		em.FlagComponents.GetComponent(e).set(ComponentFlags::AnimationEnabled);
		em.FlagComponents.GetComponent(e).set(ComponentFlags::RenderShadows);
		auto& s = em.AnimationArchetype.ShadowComponents.GetComponent(e);
		s.image = def.Art.GetShadowImage();
		s.offset = def.Art.ShadowOffset;

		EntityUtil::PlayAnimation(e, clip);


		//EntityUtil::SetImageFrame(e, img, 12, false);
		Vector2Int16 pos = Vector2Int16((i % 100) * 32 + 16, (i / 100) * 32 + 16);
		EntityUtil::SetPosition(e, pos);
		EntityUtil::SetOrientation(e, 12);
	}

	em.FullUpdate();
}
static int t = 0;
static int c = 0;


static std::vector<EntityId> v;


int i = 0;
void PerformanceTestScene::Update() {
	++i;
	em.FrameUpdate(camera);
	for (int i = 0; i < 10000; ++i) {
		EntityUtil::UpdateAnimationVisual(i); // Need optimizations
		//EntityUtil::SetPosition(i, em.PositionComponents.GetComponent(i));
	}

	//EntityUtil::SetOrientation(0, i/4);
	//EntityUtil::UpdateAnimationVisual(0);
}

void PerformanceTestScene::Draw() {
	GraphicsRenderer::DrawOnScreen(ScreenId::Top);
	em.Draw(camera);
}