#include "PerformanceTestScene.h"
#include "Data/UnitDatabase.h"
#include "../Platform.h"
#include "../Profiler.h"
#include <algorithm>
#include "../Job.h"
#include "../Entity/Component.h"


struct RenderOffsetCmp {
	Vector2Int offset = { 0,0 };
	Vector2Int shadowOffset = { 0,0 };
};

struct RenderOffsetCmp16 {
	Vector2Int16 offset = { 0,0 };
	Vector2Int16 shadowOffset = { 0,0 };
};

struct RenderPosCmp {
	Vector2Int _dst;
	Vector2Int _shadowDst;
};

struct RenderPosCmp16 {
	Vector2Int16 _dst;
	Vector2Int16 _shadowDst;
};

struct TransformCmp {
	Vector2Int pos;
};

struct TransformCmp16 {
	Vector2Int16 pos;
};

struct RenderUpdatePosArchetype {
	std::vector < RenderPosCmp*> outPos;
	std::vector < Vector2Int> worldPos;
	std::vector < RenderOffsetCmp> offset;
};


struct RenderUpdatePosArchetype16 {
	std::vector < RenderPosCmp16*> outPos;
	std::vector < Vector2Int16> worldPos;
	std::vector < RenderOffsetCmp16> offset;
};

ComponentCollection<RenderOffsetCmp> renderOffsetComponents;
ComponentCollection<RenderPosCmp> renderPosComponents;
ComponentCollection<TransformCmp> transformComponents;

RenderUpdatePosArchetype renderUpdatePosArchetype;


ComponentCollection<RenderOffsetCmp16> renderOffsetComponents16;
ComponentCollection<RenderPosCmp16> renderPosComponents16;
ComponentCollection<TransformCmp16> transformComponents16;

RenderUpdatePosArchetype16 renderUpdatePosArchetype16;



std::vector<bool> hasRenderComponents;




void UpdateOld() {

	SectionProfiler p("UpdateOld");

	renderUpdatePosArchetype.outPos.clear();
	renderUpdatePosArchetype.worldPos.clear();
	renderUpdatePosArchetype.offset.clear();


	int size = transformComponents.size();

	for (int i = 0; i < size; ++i) {
		int id = i + 1;

		if (renderPosComponents.HasComponent(id))
		{
			renderUpdatePosArchetype.outPos.push_back(&renderPosComponents.GetComponent(id));
			renderUpdatePosArchetype.worldPos.push_back(transformComponents.GetComponent(id).pos);
			renderUpdatePosArchetype.offset.push_back(renderOffsetComponents.GetComponent(id));
		}

	}

	size = renderUpdatePosArchetype.outPos.size();

	for (int i = 0; i < size; ++i) {
		auto p = renderUpdatePosArchetype.outPos[i];

		p->_dst = renderUpdatePosArchetype.worldPos[i] + renderUpdatePosArchetype.offset[i].offset;
		p->_shadowDst = renderUpdatePosArchetype.worldPos[i] + renderUpdatePosArchetype.offset[i].shadowOffset;
	}

	p.Submit();
}


void UpdateNew16() {
	SectionProfiler p("UpdateNew16");


	renderUpdatePosArchetype16.outPos.clear();
	renderUpdatePosArchetype16.worldPos.clear();
	renderUpdatePosArchetype16.offset.clear();

	int size = transformComponents16.size();

	for (int i = 0; i < size; ++i) {
		if (hasRenderComponents[i])
		{
			renderUpdatePosArchetype16.outPos.push_back(&renderPosComponents16[i]);
			renderUpdatePosArchetype16.worldPos.push_back(transformComponents16[i].pos);
			renderUpdatePosArchetype16.offset.push_back(renderOffsetComponents16[i]);
		}

	}


	size = renderUpdatePosArchetype16.outPos.size();

	for (int i = 0; i < size; ++i) {
		auto p = renderUpdatePosArchetype16.outPos[i];

		p->_dst = renderUpdatePosArchetype16.worldPos[i] + renderUpdatePosArchetype16.offset[i].offset;
		p->_shadowDst = renderUpdatePosArchetype16.worldPos[i] + renderUpdatePosArchetype16.offset[i].shadowOffset;
	}

	p.Submit();
}

void UpdateNew() {
	SectionProfiler p("UpdateNew");


	renderUpdatePosArchetype.outPos.clear();
	renderUpdatePosArchetype.worldPos.clear();
	renderUpdatePosArchetype.offset.clear();

	int size = transformComponents.size();

	for (int i = 0; i < size; ++i) {
		if (hasRenderComponents[i])
		{
			renderUpdatePosArchetype.outPos.push_back(&renderPosComponents[i]);
			renderUpdatePosArchetype.worldPos.push_back(transformComponents[i].pos);
			renderUpdatePosArchetype.offset.push_back(renderOffsetComponents[i]);
		}

	}

	size = renderUpdatePosArchetype.outPos.size();

	for (int i = 0; i < size; ++i) {
		auto p = renderUpdatePosArchetype.outPos[i];

		p->_dst = renderUpdatePosArchetype.worldPos[i] + renderUpdatePosArchetype.offset[i].offset;
		p->_shadowDst = renderUpdatePosArchetype.worldPos[i] + renderUpdatePosArchetype.offset[i].shadowOffset;
	}

	p.Submit();
}

void PerformanceTestScene::Start() {

	camera.Position = { 400,240 };
	camera.Size = { 400,240 };
	camera.Scale = 2;

	for (int i = 0; i < 100'00; ++i) {
		transformComponents.NewComponent(i + 1, {});
		renderPosComponents.NewComponent(i + 1, {});
		renderOffsetComponents.NewComponent(i + 1, {});
		hasRenderComponents.push_back(true);

		transformComponents16.NewComponent(i + 1, {});
		renderPosComponents16.NewComponent(i + 1, {});
		renderOffsetComponents16.NewComponent(i + 1, {});
	}


}
static int t = 0;


void PerformanceTestScene::Update() {

	//t++;


	UpdateOld();
	UpdateNew();
	UpdateNew16();
}



void PerformanceTestScene::Draw() {


}