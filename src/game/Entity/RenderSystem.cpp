#include "RenderSystem.h"
#include "../Profiler.h"
#include "../Job.h"
#include "../Camera.h"
#include "EntityManager.h"
#include "../Util.h"
#include "../Engine/GraphicsRenderer.h"

#include <algorithm>
#include <cstring>

void RenderSystem::CameraCull(const Rectangle16& camRect, EntityManager& em) {

	SectionProfiler p("CameraCull");

	renderData.clear();

	// TODO: second camera cull for more accurate sprite culling???

	for (EntityId id : em.RenderArchetype.Archetype.GetEntities()) {

		int i = Entity::ToIndex(id);

		if (!em.FlagComponents[i].test(ComponentFlags::RenderEnabled))
			continue;

		const Rectangle16& bb = em.RenderArchetype.BoundingBoxComponents[i];

		if (!camRect.Intersects(bb))
			continue;

		renderData.pos.push_back(em.RenderArchetype.DestinationComponents[i]);
		renderData.ren.push_back(em.RenderArchetype.RenderComponents[i]);
	}

	renderUnitData.clear();

	for (EntityId id : em.UnitArchetype.RenderArchetype.Archetype.GetEntities()) {

		auto& arch = em.UnitArchetype.RenderArchetype;

		int i = Entity::ToIndex(id);

		if (!em.FlagComponents[i].test(ComponentFlags::RenderEnabled))
			continue;

		const Rectangle16& bb = arch.BoundingBoxComponents[i];

		if (!camRect.Intersects(bb))
			continue;

		renderUnitData.pos.push_back(arch.DestinationComponents[i]);
		renderUnitData.ren.push_back(arch.RenderComponents[i]);

	}

	UnitSelectionCameraCull(camRect, em);
}

void RenderSystem::UnitSelectionCameraCull(const Rectangle16& camRect, EntityManager& em) {
	unitSelectionData.clear();

	int j = -1;
	for (EntityId id : selectedUnits) {
		++j;

		auto& arch = em.UnitArchetype.RenderArchetype;

		int i = Entity::ToIndex(id);

		if (!em.FlagComponents[i].test(ComponentFlags::RenderEnabled))
			continue;

		const Rectangle16& bb = arch.BoundingBoxComponents[i];

		if (!camRect.Intersects(bb))
			continue;

		const auto* def = em.UnitArchetype.UnitComponents.GetComponent(id).def;

		unitSelectionData.graphics.push_back(def->Graphics->Selection.Atlas->GetFrame(0));
		Vector2Int16 pos = em.PositionComponents.GetComponent(id);
		pos -= Vector2Int16(def->Graphics->Selection.Atlas->FrameSize / 2);
		unitSelectionData.position.push_back(pos);
		unitSelectionData.order.push_back(arch.DestinationComponents.GetComponent(id).order);
		unitSelectionData.verticalOffset.push_back(def->Graphics->Selection.VecticalOffset);
		unitSelectionData.color.push_back(selectionColor[j]);

	}
}

void RenderSystem::DrawEntities(const Camera& camera, const Rectangle16& camRect) {

	float camMul = 1.0f / camera.Scale;

	Vector2 scale[] = { {camMul,camMul},{-camMul,camMul} };

	int entitiesCount = renderData.size();

	for (int i = 0; i < entitiesCount; ++i) {
		Vector2Int16  dst = renderData.pos[i].dst;
		const auto& r = renderData.ren[i];

		dst -= camRect.position;
		dst /= camera.Scale;


		BatchDrawCommand cmd;
		cmd.order = renderData.pos[i].order;
		cmd.sprite = r.sprite;
		cmd.position = dst;
		cmd.scale = scale[r.hFlip];
		cmd.color = Color32(Colors::White);

		render.push_back(cmd);
	}
}
void RenderSystem::DrawUnits(const Camera& camera, const Rectangle16& camRect) {
	constexpr const Color32 shadowColor = Color32(0.0f, 0.0f, 0.0f, 0.5f);

	float camMul = 1.0f / camera.Scale;

	Vector2 scale[] = { {camMul,camMul},{-camMul,camMul} };

	int entitiesCount = renderUnitData.size();

	for (int i = 0; i < entitiesCount; ++i) {
		const auto& rp = renderUnitData.pos[i];
		const auto& r = renderUnitData.ren[i];

		Vector2Int16 dst = rp.dst;
		dst -= camRect.position;
		dst /= camera.Scale;

		Vector2Int16 shadowDst = rp.shadowDst;
		shadowDst -= camRect.position;
		shadowDst /= camera.Scale;


		BatchDrawCommand cmd;
		cmd.order = rp.order;
		cmd.sprite = r.shadowSprite;
		cmd.position = shadowDst;
		cmd.scale = scale[r.hFlip];
		cmd.color = shadowColor;
		if (cmd.sprite.image.textureId)
			render.push_back(cmd);

		cmd.order += 2;
		cmd.sprite = r.sprite;
		cmd.position = dst;
		cmd.color = Color32(Colors::White);

		render.push_back(cmd);

		cmd.order++;
		cmd.sprite = r.colorSprite;
		cmd.color = Color32(r.unitColor);
		if (cmd.sprite.image.textureId)
			render.push_back(cmd);
	}
}
void RenderSystem::DrawSelection(const Camera& camera, const Rectangle16& camRect) {

	int entitiesCount = unitSelectionData.size();

	float camMul = 1.0f / camera.Scale;
	BatchDrawCommand cmd;
	cmd.scale = Vector2(camMul);


	for (int i = 0; i < entitiesCount; ++i) {
		auto& dst = unitSelectionData.position[i];
		const auto& frame = unitSelectionData.graphics[i];
		const short& offset = unitSelectionData.verticalOffset[i];

		dst += frame.offset;
		dst.y += offset;

		dst -= camRect.position;
		dst /= camera.Scale;

		cmd.order = unitSelectionData.order[i] + 1;
		cmd.sprite = frame.sprite;
		cmd.position = dst;
		cmd.color = unitSelectionData.color[i];

		render.push_back(cmd);
	}
}

void RenderSystem::Draw(const Camera& camera, EntityManager& em) {
	Rectangle16 camRect = camera.GetRectangle16();

	CameraCull(camRect, em);

	SectionProfiler p("EntityDraw");

	render.clear();

	DrawEntities(camera, camRect);
	DrawUnits(camera, camRect);
	DrawSelection(camera, camRect);

	std::sort(render.begin(), render.end(), RenderSort);

	GraphicsRenderer::BufferDraw(render);
}
void RenderSystem::DrawBoundingBoxes(const Camera& camera, EntityManager& em) {


	Rectangle16 camRect = camera.GetRectangle16();

	Color c = Colors::UIGreen;
	c.a = 0.5f;

	for (EntityId id : em.UnitArchetype.RenderArchetype.Archetype.GetEntities()) {

		auto& arch = em.UnitArchetype.RenderArchetype;

		int i = Entity::ToIndex(id);

		if (!em.FlagComponents[i].test(ComponentFlags::RenderEnabled))
			continue;

		Rectangle16 bb = arch.BoundingBoxComponents[i];

		if (!camRect.Intersects(bb))
			continue;

		bb.size /= camera.Scale;
		bb.position -= camRect.position;
		bb.position /= camera.Scale;

		Util::DrawTransparentRectangle(bb, c);
	}

	for (EntityId id : em.RenderArchetype.Archetype.GetEntities()) {

		auto& arch = em.RenderArchetype;

		int i = Entity::ToIndex(id);

		if (!em.FlagComponents[i].test(ComponentFlags::RenderEnabled))
			continue;

		Rectangle16 bb = arch.BoundingBoxComponents[i];

		if (!camRect.Intersects(bb))
			continue;

		bb.size /= camera.Scale;
		bb.position -= camRect.position;
		bb.position /= camera.Scale;

		Util::DrawTransparentRectangle(bb, c);
	}
}

bool RenderSystem::RenderSort(const BatchDrawCommand& a, const BatchDrawCommand& b) {
	return a.order < b.order;
}

static RenderSystem* s;

void RenderSystem::UpdateRenderPositionsJob(int start, int end) {
	RenderUpdatePosData& data = s->updatePosData;

	for (int i = start; i < end; ++i) {
		RenderDestinationComponent& p = *data.outDst[i];
		p.dst = data.worldPos[i] + data.offset[i];
		p.order = data.depth[i] * 10'000'000 + p.dst.y * 1000 + p.dst.x * 4;

		data.outBB[i]->SetCenter(data.worldPos[i]);
	}
}
void RenderSystem::UpdateUnitRenderPositionsJob(int start, int end) {
	RenderUnitUpdatePosData& data = s->unitUpdatePosData;

	for (int i = start; i < end; ++i) {
		RenderUnitDestinationComponent& p = *data.outPos[i];
		p.dst = data.worldPos[i] + data.offset[i].offset;
		p.shadowDst = data.worldPos[i] + data.offset[i].shadowOffset;
		p.order = data.depth[i] * 10'000'000 + p.dst.y * 1000 + p.dst.x * 4;

		data.outBB[i]->SetCenter(data.worldPos[i]);
	}
}

void RenderSystem::UpdatePositions(EntityManager& em, const EntityChangedData& changed) {
	updatePosData.clear();
	unitUpdatePosData.clear();

	for (EntityId id : changed.entity) {
		if (em.RenderArchetype.Archetype.HasEntity(id) ||
			em.UnitArchetype.RenderArchetype.Archetype.HasEntity(id))

			em.FlagComponents.GetComponent(id).set(ComponentFlags::RenderChanged);
	}

	for (EntityId id : em.RenderArchetype.Archetype.GetEntities()) {
		int i = Entity::ToIndex(id);
		FlagsComponent& flag = em.FlagComponents[i];

		if (!flag.test(ComponentFlags::RenderEnabled))
			continue;

		auto& arch = em.RenderArchetype;

		if (flag.test(ComponentFlags::RenderChanged)) {

			flag.clear(ComponentFlags::RenderChanged);
			updatePosData.outDst.push_back(&arch.DestinationComponents[i]);
			updatePosData.worldPos.push_back(em.PositionComponents[i]);
			updatePosData.offset.push_back(arch.OffsetComponents[i]);
			updatePosData.outBB.push_back(&arch.BoundingBoxComponents[i]);
			updatePosData.depth.push_back(arch.RenderComponents[i].depth);
		}
	}

	for (EntityId id : em.UnitArchetype.RenderArchetype.Archetype.GetEntities()) {

		int i = Entity::ToIndex(id);
		FlagsComponent& flag = em.FlagComponents[i];

		if (!flag.test(ComponentFlags::RenderEnabled))
			continue;

		auto& arch = em.UnitArchetype.RenderArchetype;

		if (flag.test(ComponentFlags::RenderChanged)) {

			flag.clear(ComponentFlags::RenderChanged);
			unitUpdatePosData.outPos.push_back(&arch.DestinationComponents[i]);
			unitUpdatePosData.worldPos.push_back(em.PositionComponents[i]);
			unitUpdatePosData.offset.push_back(arch.OffsetComponents[i]);
			unitUpdatePosData.outBB.push_back(&arch.BoundingBoxComponents[i]);
			unitUpdatePosData.depth.push_back(arch.RenderComponents[i].depth);
		}
	}

	s = this;

	JobSystem::RunJob(updatePosData.size(), JobSystem::DefaultJobSize, UpdateRenderPositionsJob);
	JobSystem::RunJob(unitUpdatePosData.size(), JobSystem::DefaultJobSize, UpdateUnitRenderPositionsJob);
}

void RenderSystem::ClearSelection() {
	selectedUnits.clear();
	selectionColor.clear();
}

void RenderSystem::AddSelection(const std::vector<EntityId>& selection, Color color)
{
	selectedUnits.insert(selectedUnits.end(), selection.begin(), selection.end());

	for (int i = 0; i < selection.size(); ++i) {
		selectionColor.push_back(Color32(color));
	}
}
void RenderSystem::AddSelection(EntityId id, Color color)
{
	selectedUnits.push_back(id);
	selectionColor.push_back(Color32(color));
}

