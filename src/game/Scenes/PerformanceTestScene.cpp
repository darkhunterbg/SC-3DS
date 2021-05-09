#include "PerformanceTestScene.h"
#include "Data/UnitDatabase.h"
#include "../Platform.h"
#include "../Profiler.h"
#include <algorithm>
#include "../Job.h"
#include "../Entity/Component.h"


struct Test {
	Vector2Int16 a, b;
};

ComponentCollection< Test> components;


void PerformanceTestScene::Start() {

	camera.Position = { 400,240 };
	camera.Size = { 400,240 };
	camera.Scale = 2;

	for (int i = 0; i < 10000; ++i) {
		components.NewComponent(i + 1);
	}

	/*for (int i = 0; i < 100'00; ++i) {
		transformComponents.NewComponent(i + 1, {});
		renderPosComponents.NewComponent(i + 1, {});
		renderOffsetComponents.NewComponent(i + 1, {});
		hasRenderComponents.push_back(true);

		transformComponents16.NewComponent(i + 1, {});
		renderPosComponents16.NewComponent(i + 1, {});
		renderOffsetComponents16.NewComponent(i + 1, {});
	}*/


}
static int t = 0;

std::vector<Test> c;

void PerformanceTestScene::Update() {
	c.clear();
	SectionProfiler p("Iterate");
	for (int i = 0; i < 10000; ++i) {
		//if (components.HasComponent(i + 1))
		{
			c.push_back(components.GetComponent(i + 1));
		}
	}
	p.Submit();
}



void PerformanceTestScene::Draw() {


}