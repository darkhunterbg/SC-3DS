#include "RenderSystem.h"
#include "../Platform.h"
#include "../Profiler.h"
#include "../Job.h"
#include "../Camera.h"
#include "EntityManager.h"

#include <algorithm>
#include <cstring>

void RenderSystem::CameraCull(const Rectangle16& camRect, EntityManager& em) {

	SectionProfiler p("CameraCull");

	renderArchetype.clear();

	for (EntityId id : em.RenderArchetype.GetEntities()) {

		int i = Entity::ToIndex(id);
		const Rectangle16& bb = em.RenderBoundingBoxComponents[i];

		if (!camRect.Intersects(bb))
			continue;

		renderArchetype.pos.push_back(em.RenderDestinationComponents[i]);
		renderArchetype.ren.push_back(em.RenderComponents[i]);

	}

	p.Submit();
}

void RenderSystem::Draw(const Camera& camera, EntityManager& em) {

	SectionProfiler p("RenderEntities");

	Rectangle16 camRect = camera.GetRectangle16();

	CameraCull(camRect, em);

	constexpr const Color4 shadowColor = Color4(0.0f, 0.0f, 0.0f, 0.5f);

	float camMul = 1.0f / camera.Scale;

	render.clear();

	Vector2 scale[] = { {camMul,camMul},{-camMul,camMul} };

	int entitiesCount = renderArchetype.size();

	for (int i = 0; i < entitiesCount; ++i) {
		const auto& rp = renderArchetype.pos[i];
		const auto& r = renderArchetype.ren[i];

		Vector2Int16 dst = rp.dst;
		dst -= camRect.position;
		dst /= camera.Scale;

		Vector2Int16 shadowDst = rp.shadowDst;
		shadowDst -= camRect.position;
		shadowDst /= camera.Scale;

		int order = r.depth * 10'000'000;
		order += dst.y * 1000 + dst.x * 3;


		BatchDrawCommand cmd;
		cmd.order = order;
		cmd.image = r.shadowSprite;
		cmd.position = shadowDst;
		cmd.scale = scale[r.hFlip];
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

	p.Submit();
}

bool RenderSystem::RenderSort(const BatchDrawCommand& a, const BatchDrawCommand& b) {
	return a.order < b.order;
}


static RenderUpdatePositionArchetype* a;

static void SetPosition(int start, int end) {
	RenderUpdatePositionArchetype& archetype = *a;

	for (int i = start; i < end; ++i) {
		RenderDestinationComponent* p = archetype.outPos[i];
		p->dst = archetype.worldPos[i] + archetype.offset[i].offset;
		p->shadowDst = archetype.worldPos[i] + archetype.offset[i].shadowOffset;
		archetype.outBB[i]->SetCenter(archetype.worldPos[i]);
	}
}

void RenderSystem::UpdatePositions(EntityManager& em) {

	SectionProfiler p("RenderUpdate");

	renderUpdatePosArchetype.clear();

	for (EntityId id : em.GetEntities()) {
		int i = Entity::ToIndex(id);

		if (em.EntityChangeComponents[i].changed) {
			em.EntityChangeComponents[i].changed = false;

			if (em.RenderArchetype.HasEntity(id))
			{
				renderUpdatePosArchetype.outPos.push_back(&em.RenderDestinationComponents[i]);
				renderUpdatePosArchetype.worldPos.push_back(em.PositionComponents[i]);
				renderUpdatePosArchetype.offset.push_back(em.RenderOffsetComponents[i]);
				renderUpdatePosArchetype.outBB.push_back(&em.RenderBoundingBoxComponents[i]);
			}

		}
	}

	int size = renderUpdatePosArchetype.size();
	a = &renderUpdatePosArchetype;
	JobSystem::RunJob(size, JobSystem::DefaultJobSize, SetPosition);

	p.Submit();
}

