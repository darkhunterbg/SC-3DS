#include "RenderSystem.h"
#include "../Platform.h"
#include "../Profiler.h"
#include "../Job.h"
#include "../Camera.h"
#include "EntityManager.h"

#include <algorithm>
#include <cstring>

void RenderSystem::CameraCull(const Rectangle16& camRect, EntityManager& em) {
	renderData.clear();

	for (EntityId id : em.RenderArchetype.Archetype.GetEntities()) {

		int i = Entity::ToIndex(id);
		const Rectangle16& bb = em.RenderArchetype.BoundingBoxComponents[i];

		if (!camRect.Intersects(bb))
			continue;

		renderData.pos.push_back(em.RenderArchetype.DestinationComponents[i]);
		renderData.ren.push_back(em.RenderArchetype.RenderComponents[i]);

	}
}

void RenderSystem::Draw(const Camera& camera, EntityManager& em) {
	Rectangle16 camRect = camera.GetRectangle16();

	CameraCull(camRect, em);

	constexpr const Color4 shadowColor = Color4(0.0f, 0.0f, 0.0f, 0.5f);

	float camMul = 1.0f / camera.Scale;

	render.clear();

	Vector2 scale[] = { {camMul,camMul},{-camMul,camMul} };

	int entitiesCount = renderData.size();

	for (int i = 0; i < entitiesCount; ++i) {
		const auto& rp = renderData.pos[i];
		const auto& r = renderData.ren[i];

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
}

bool RenderSystem::RenderSort(const BatchDrawCommand& a, const BatchDrawCommand& b) {
	return a.order < b.order;
}


static RenderSystem* s;

void RenderSystem::UpdateRenderPositionsJob(int start, int end) {
	RenderUpdatePosData& data = s->renderUpdatePosData;

	for (int i = start; i < end; ++i) {
		RenderDestinationComponent* p = data.outPos[i];
		p->dst = data.worldPos[i] + data.offset[i].offset;
		p->shadowDst = data.worldPos[i] + data.offset[i].shadowOffset;
		data.outBB[i]->SetCenter(data.worldPos[i]);
	}
}

void RenderSystem::UpdatePositions(EntityManager& em, const EntityChangedData& changed) {

	renderUpdatePosData.clear();

	int size = changed.size();
	for (int item = 0; item < size; ++item) {
		EntityId id = changed.entity[item];

		if (em.RenderArchetype.Archetype.HasEntity(id))
		{
			int i = Entity::ToIndex(id);

			renderUpdatePosData.outPos.push_back(&em.RenderArchetype.DestinationComponents[i]);
			renderUpdatePosData.worldPos.push_back(changed.position[item]);
			renderUpdatePosData.offset.push_back(em.RenderArchetype.OffsetComponents[i]);
			renderUpdatePosData.outBB.push_back(&em.RenderArchetype.BoundingBoxComponents[i]);
		}
	}


	s = this;
	JobSystem::RunJob(renderUpdatePosData.size(), JobSystem::DefaultJobSize, UpdateRenderPositionsJob);
}

