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

	camera.Position = { 200 ,120 };
	camera.Size = { 400,240 };
	camera.Scale = 1;

	em.Init({ 128 * 32,128 * 32 });
	em.GetPlayerSystem().AddPlayer(RaceDatabase::Terran, Colors::White);
	em.GetMapSystem().FogOfWarVisible = false;
	//em.DrawBoundingBoxes = true;

	const auto& img = GameDatabase::instance->GetImage("unit\\terran\\marine");
	const auto& clip = GameDatabase::instance->AnimClipDefs[1];

	for (int i = 0; i < 2000; ++i) {
		EntityId e = em.NewEntity();
		em.RenderArchetype.Archetype.AddEntity(e);
		em.AnimationArchetype.Archetype.AddEntity(e);
		em.FlagComponents.GetComponent(e).set(ComponentFlags::RenderEnabled);
		em.FlagComponents.GetComponent(e).set(ComponentFlags::AnimationEnabled);
		auto& anim = em.AnimationArchetype.AnimationComponents.NewComponent(e);
		anim.baseImage = &img;
		anim.instructionStart = clip.instructionStart;
		anim.instructionEnd = clip.instructionStart + clip.instructionCount;
		em.AnimationArchetype.StateComponents.NewComponent(e).instructionId = clip.instructionStart;
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

	//EntityUtil::SetOrientation(0, i/4);
	EntityUtil::UpdateAnimationVisual(0);
}

void PerformanceTestScene::Draw() {
	GraphicsRenderer::DrawOnScreen(ScreenId::Top);
	em.Draw(camera);
}