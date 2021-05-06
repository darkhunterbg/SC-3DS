#include "PerformanceTestScene.h"
#include "Data/UnitDatabase.h"
#include "../Platform.h"
#include "../Profiler.h"
#include <algorithm>
#include "../Job.h"

struct RenderCmp {
	Color4 unitColor;
	Image sprite;
	Image shadowSprite;
	Image colorSprite;
	int depth = 0;
	bool hFlip = false;
};

struct RenderOffsetCmp {
	Vector2Int offset = { 0,0 };
	Vector2Int shadowOffset = { 0,0 };
};

struct RenderPosCmp {
	Vector2Int _dst;
	Vector2Int _shadowDst;
};


struct TransformCmp {
	Vector2Int pos;
};


struct RenderArchetype {
	std::vector < RenderPosCmp> pos;
	std::vector < RenderCmp> ren;
};
struct RenderUpdatePosArchetype {
	std::vector < RenderPosCmp*> outPos;
	std::vector < Vector2Int> worldPos;
	std::vector < RenderOffsetCmp> offset;
};

std::vector<RenderCmp> renderComponents;
std::vector<RenderOffsetCmp> renderOffsetComponents;
std::vector<RenderPosCmp> renderPosComponents;
std::vector<TransformCmp> transformComponents;

std::vector<BatchDrawCommand> render;

std::vector<bool> entityChanged;

RenderArchetype renderArchetype;
RenderUpdatePosArchetype renderUpdatePosArchetype;

void NewEntity(const UnitDef& unit, Vector2Int pos, Color color) {

	RenderCmp r = {
		Color4(color),
		unit.MovementAnimations[0].GetFrame(0).sprite.image,
		unit.MovementAnimationsShadow[0].GetFrame(0).sprite.image,
		unit.MovementAnimationsTeamColor[0].GetFrame(0).sprite.image,
		0,
		unit.MovementAnimationsTeamColor[0].GetFrame(0).hFlip,
	};
	RenderOffsetCmp ro = {
		unit.MovementAnimations[0].GetFrame(0).offset,
		unit.MovementAnimationsShadow[0].GetFrame(0).offset,
	};
	RenderPosCmp rp = {
		 pos + ro.offset,
		 pos + ro.shadowOffset
	};

	renderComponents.push_back(r);
	renderOffsetComponents.push_back(ro);
	renderPosComponents.push_back(rp);
	transformComponents.push_back({ pos });
	entityChanged.push_back(false);

}

void SetPosition(int i, Vector2Int pos) {
	entityChanged[i] = true;
	transformComponents[i].pos = pos;
}
bool RenderSort(const BatchDrawCommand& a, const BatchDrawCommand& b) {
	return a.order < b.order;
}

ThreadLocal<RenderArchetype>* tlRenderAchetype;
//std::vector<int> architypeToEntity;

void Update(int start, int end) {
	for (int i = start; i < end; ++i) {
		auto p = renderUpdatePosArchetype.outPos[i];

		p->_dst = renderUpdatePosArchetype.worldPos[i] + renderUpdatePosArchetype.offset[i].offset;
		p->_shadowDst = renderUpdatePosArchetype.worldPos[i] + renderUpdatePosArchetype.offset[i].shadowOffset;
	}
};


ThreadLocal<RenderUpdatePosArchetype>* tlsRenderUpdatePosArchetype;

void UpdateEntities() {
	renderUpdatePosArchetype.outPos.clear();
	renderUpdatePosArchetype.worldPos.clear();
	renderUpdatePosArchetype.offset.clear();

	int size = transformComponents.size();
	//architypeToEntity.clear();

	for (auto& r : tlsRenderUpdatePosArchetype->GetAll()) {
		r.outPos.clear();
		r.worldPos.clear();
		r.offset.clear();
	}

	//JobSystem::RunJob(size, 128, [](int start, int end) {
	//	for (int i = start; i < end; ++i) {

	//		if (entityChanged[i]) {
	//			entityChanged[i] = false;

	//			tlsRenderUpdatePosArchetype->Get().outPos.push_back(&renderPosComponents[i]);
	//			tlsRenderUpdatePosArchetype->Get().worldPos.push_back(transformComponents[i].pos);
	//			tlsRenderUpdatePosArchetype->Get().offset.push_back(renderOffsetComponents[i]);
	//		}
	//	}
	//	});

	//for (const auto& r : tlsRenderUpdatePosArchetype->GetAll()) {
	//	renderUpdatePosArchetype.outPos.insert(renderUpdatePosArchetype.outPos.begin(), r.outPos.begin(), r.outPos.end());
	//	renderUpdatePosArchetype.worldPos.insert(renderUpdatePosArchetype.worldPos.begin(), r.worldPos.begin(), r.worldPos.end());
	//	renderUpdatePosArchetype.offset.insert(renderUpdatePosArchetype.offset.begin(), r.offset.begin(), r.offset.end());
	//}

	for (int i = 0; i < size; ++i) {

		if (entityChanged[i]) {
			entityChanged[i] = false;

			renderUpdatePosArchetype.outPos.push_back(&renderPosComponents[i]);
			renderUpdatePosArchetype.worldPos.push_back(transformComponents[i].pos);
			renderUpdatePosArchetype.offset.push_back(renderOffsetComponents[i]);
		}
	}

	size = renderUpdatePosArchetype.outPos.size();

	JobSystem::RunJob(size, 128, Update);

	//for (int i = 0; i < size; ++i) {
	//	auto p = renderUpdatePosArchetype.outPos[i];
	//	p->_dst = renderUpdatePosArchetype.worldPos[i] + renderUpdatePosArchetype.offset[i].offset;
	//	p->_shadowDst = renderUpdatePosArchetype.worldPos[i] + renderUpdatePosArchetype.offset[i].shadowOffset;
	//}

}

ThreadLocal<std::vector<BatchDrawCommand>>* tlRender;

void DrawEntities(const Camera& camera) {

	int entitiesCount = renderArchetype.pos.size();

	Rectangle camRect = camera.GetRectangle();

	static constexpr Color4 shadowColor = Color4(0.0f, 0.0f, 0.0f, 0.5f);

	float camMul = 1.0f / camera.Scale;

	render.clear();

	/*for (auto& r : tlRender->GetAll())
		r.clear();

	JobSystem::RunJob(entitiesCount, 128, [camera, camRect, camMul](int start, int end) {
		for (int i = start; i < end; ++i) {
			const auto& rp = renderArchetype.pos[i];
			const auto& r = renderArchetype.ren[i];

			Vector2Int dst = rp._dst;
			dst -= camRect.position;
			dst /= camera.Scale;

			Vector2Int shadowDst = rp._shadowDst;
			shadowDst -= camRect.position;
			shadowDst /= camera.Scale;

			int order = r.depth * 10'000'000;
			order += dst.y * 1000 + dst.x * 3;

			Vector2 flip = { r.hFlip ? -1.0f : 1.0f,1.0f };

			BatchDrawCommand cmd;
			cmd.order = order;
			cmd.image = r.shadowSprite;
			cmd.position = shadowDst;
			cmd.scale = flip * camMul;
			cmd.color = { shadowColor, 1 };
			tlRender->Get().push_back(cmd);

			cmd.order++;
			cmd.image = r.sprite;
			cmd.position = dst;
			cmd.color = { Color4(Colors::Black),0 };
			tlRender->Get().push_back(cmd);

			cmd.order++;
			cmd.image = r.colorSprite;
			cmd.color = { Color4(r.unitColor), 0.66f };
			tlRender->Get().push_back(cmd);
		}

		});

	for (const auto& r : tlRender->GetAll()) {
		render.insert(render.begin(), r.begin(), r.end());
	}*/

	for (int i = 0; i < entitiesCount; ++i) {
		const auto& rp = renderArchetype.pos[i];
		const auto& r = renderArchetype.ren[i];

		Vector2Int dst = rp._dst;
		dst -= camRect.position;
		dst /= camera.Scale;

		Vector2Int shadowDst = rp._shadowDst;
		shadowDst -= camRect.position;
		shadowDst /= camera.Scale;

		int order = r.depth * 10'000'000;
		order += dst.y * 1000 + dst.x * 3;

		Vector2 flip = { r.hFlip ? -1.0f : 1.0f,1.0f };

		BatchDrawCommand cmd;
		cmd.order = order;
		cmd.image = r.shadowSprite;
		cmd.position = shadowDst;
		cmd.scale = flip * camMul;
		cmd.color = { shadowColor, 1 };
		render.push_back(cmd);

		cmd.order++;
		cmd.image = r.sprite;
		cmd.position = dst;
		cmd.color = { Color4(Colors::Black),0 };
		render.push_back(cmd);

		cmd.order++;
		cmd.image = r.colorSprite;
		cmd.color = { Color4(r.unitColor), 0.66f };
		render.push_back(cmd);
	}

	std::sort(render.begin(), render.end(), RenderSort);


	Platform::BatchDraw({ render.data(),render.size() });
}

void PerformanceTestScene::Start() {

	tlRenderAchetype = new ThreadLocal< RenderArchetype>();
	tlRender = new ThreadLocal<std::vector<BatchDrawCommand>>();
	tlsRenderUpdatePosArchetype = new ThreadLocal<RenderUpdatePosArchetype>();

	int a = sizeof(RenderArchetype);
	int b = sizeof(std::vector<BatchDrawCommand>);

	UnitDatabase::LoadAllUnitResources();

	camera.Position = { 400,240 };
	camera.Size = { 400,240 };
	camera.Scale = 2;

	Color color[] = { Colors::SCRed, Colors::SCBlue, Colors::SCLightGreen, Colors::SCPurle,
 Colors::SCOrange, Colors::SCGreen, Colors::SCBrown, Colors::SCLightYellow, Colors::SCWhite,
Colors::SCTeal , Colors::SCYellow , Colors::SCLightBlue };

	int i = 0;
	for (int y = 99; y >= 0; --y) {
		for (int x = 99; x >= 0; --x) {

			Color c = color[(i++) % 12];
			NewEntity(*UnitDatabase::Units[i % UnitDatabase::Units.size()], { x * 32 ,y * 32 }, c);
		}
	}


}
static int t = 0;


void PerformanceTestScene::Update() {

	//t++;

	SectionProfiler p("Update");

	// Difference is not big enough to matter

	//JobSystem::RunJob(10000, 128, [](int start, int end) {
	//	for (int i = start; i < end; ++i) {
	//		SetPosition(i, { (i / 100) * 32 + t ,(i % 100) * 32 + t });
	//	}
	//	});


	int i = 0;
	for (int y = 99; y >= 0; --y) {
		for (int x = 99; x >= 0; --x) {


			SetPosition(i++, { x * 32 + t ,y * 32 + t });
		}
	}


	UpdateEntities();

	p.Submit();
}



void PerformanceTestScene::Draw() {

	SectionProfiler p("Draw");
	Platform::DrawOnScreen(ScreenId::Top);

	renderArchetype.pos.clear();
	renderArchetype.ren.clear();

	Rectangle camRect = camera.GetRectangle();

	for (RenderArchetype& r : tlRenderAchetype->GetAll()) {
		r.pos.clear();
		r.ren.clear();
	}


	int size = renderComponents.size();

	// Not much difference to matter

	//JobSystem::RunJob(size, 128, [camRect](int start, int end) {

	//	for (int i = start; i < end; ++i) {
	//		const auto& rp = renderPosComponents[i];

	//		if (!camRect.Contains(rp._dst))
	//			continue;

	//		tlRenderAchetype->Get().pos.push_back(rp);
	//		tlRenderAchetype->Get().ren.push_back(renderComponents[i]);
	//	}

	//	});

	//for (RenderArchetype& r : tlRenderAchetype->GetAll()) {
	//	renderArchetype.pos.insert(renderArchetype.pos.begin(), r.pos.begin(), r.pos.end());
	//	renderArchetype.ren.insert(renderArchetype.ren.begin(), r.ren.begin(), r.ren.end());
	//}

	for (int i = 0; i < size; ++i) {
		const auto& rp = renderPosComponents[i];


		if (!camRect.Contains(rp._dst))
			continue;

		renderArchetype.pos.push_back(rp);
		renderArchetype.ren.push_back(renderComponents[i]);
	}

	DrawEntities(camera);

	p.Submit();
}