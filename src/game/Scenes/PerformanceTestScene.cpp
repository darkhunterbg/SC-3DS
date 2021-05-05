#include "PerformanceTestScene.h"
#include "Data/UnitDatabase.h"
#include "../Platform.h"

void PerformanceTestScene::Start() {

	UnitDatabase::LoadAllUnitResources();

}

void PerformanceTestScene::Update() {

}

void PerformanceTestScene::Draw() {
	Platform::DrawOnScreen(ScreenId::Top);

	auto s = UnitDatabase::Marine.MovementAnimations[0].GetFrame(0).sprite;
	for (int i = 0; i < 1000; ++i) {
		Platform::Draw(s, s.rect, Colors::White);
	}
}