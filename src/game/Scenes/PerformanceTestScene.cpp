#include "PerformanceTestScene.h"
#include "Data/UnitDatabase.h"
#include "../Platform.h"
#include "../Profiler.h"

std::array<BatchDrawCommand, 2000> draws;

void PerformanceTestScene::Start() {

	UnitDatabase::LoadAllUnitResources();


	auto s = UnitDatabase::Marine.MovementAnimations[0].GetFrame(0).sprite;

	for (int i = 0; i < 2000; ++i) {
		draws[i] = { 0, s.image, {0,0},  {1,1}, {0, 0} };
		draws[i].color.AlphaBlend(1.0f);
	}

}

void PerformanceTestScene::Update() {

}



void PerformanceTestScene::Draw() {
	Platform::DrawOnScreen(ScreenId::Top);

	SectionProfiler p("Draw");


	//for (int i = 0; i < 2000; ++i) {
	//	const auto& cmd = draws[i];
	//	Platform::TestDraw(cmd.image,cmd.position,cmd.scale.x);
	//}
	Platform::BatchDraw({ draws.data(),2000 });
	

	p.Submit();
}