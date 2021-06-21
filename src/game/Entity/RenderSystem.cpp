#include "RenderSystem.h"
#include "../Profiler.h"
#include "../Engine/JobSystem.h"
#include "../Camera.h"
#include "EntityManager.h"
#include "../Util.h"
#include "../Engine/GraphicsRenderer.h"

#include <algorithm>
#include <cstring>

void RenderSystem::CameraCull(const Rectangle16& camRect, EntityManager& em) {

	renderData.clear();

	// TODO: second camera cull for more accurate sprite culling???

	for (EntityId id : em.RenderArchetype.Archetype.GetEntities()) {

		int i = Entity::ToIndex(id);

		const auto& flags = em.FlagComponents[i];

		if (!flags.test(ComponentFlags::RenderEnabled))
			continue;

		const Rectangle16& bb = em.RenderArchetype.BoundingBoxComponents[i];

		if (!camRect.Intersects(bb))
			continue;

		renderData.entities.push_back(id);
	}

	UnitSelectionCameraCull(camRect, em);
}

void RenderSystem::UnitSelectionCameraCull(const Rectangle16& camRect, EntityManager& em) {
	unitSelectionData.clear();

	int j = -1;
	for (EntityId id : selectedUnits) {
		++j;

		auto& arch = em.RenderArchetype;

		int i = Entity::ToIndex(id);

		if (!em.FlagComponents[i].test(ComponentFlags::RenderEnabled))
			continue;

		const Rectangle16& bb = arch.BoundingBoxComponents[i];

		if (!camRect.Intersects(bb))
			continue;

		unitSelectionData.entities.push_back(id);
		unitSelectionData.color.push_back(selectionColor[j]);
	}
}

void RenderSystem::DrawEntities(const Camera& camera, const Rectangle16& camRect, EntityManager& em) {

	Color32 shadColor = Color32(0, 0, 0, 0.5f);

	float camMul = 1.0f / camera.Scale;

	Vector2 scale[] = { {camMul,camMul},{-camMul,camMul} };

	int entitiesCount = renderData.size();

	for (int i = 0; i < entitiesCount; ++i) {
		EntityId id = renderData.entities[i];
		
		const auto& dst = em.RenderArchetype.DestinationComponents.GetComponent(id);
		const auto& ren = em.RenderArchetype.RenderComponents.GetComponent(id);
		Vector2Int16 pos = dst.dst;

		pos -= camRect.position;
		pos /= camera.Scale;

		BatchDrawCommand cmd;

		if (em.FlagComponents.GetComponent(id).test(ComponentFlags::RenderShadows)) {

			const auto& shad = em.RenderArchetype.ShadowComponents.GetComponent(id);
			Vector2Int16 shadPos = dst.shadowDst;
			shadPos -= camRect.position;
			shadPos /= camera.Scale;

			cmd.order = dst.order ;
			cmd.sprite = &shad.sprite;
			cmd.position = shadPos;
			cmd.scale = scale[ren.hFlip];
			cmd.color = shadColor;

			render.push_back(cmd);
		}

		cmd.order = dst.order + 2;
		cmd.sprite = &ren.sprite;
		cmd.position = pos;
		cmd.scale = scale[ren.hFlip];
		cmd.color = 0xFFFFFFFF;

		render.push_back(cmd);

		if (ren.colorMask) {
			++cmd.order;
			cmd.sprite = ren.colorMask;
			cmd.color = ren.color;
			render.push_back(cmd);
		}

	
	}
}
void RenderSystem::DrawSelection(const Camera& camera, const Rectangle16& camRect, EntityManager& em) {

	int entitiesCount = unitSelectionData.size();

	float camMul = 1.0f / camera.Scale;
	BatchDrawCommand cmd;
	cmd.scale = Vector2(camMul);


	for (int i = 0; i < entitiesCount; ++i) {
		EntityId id = unitSelectionData.entities[i];

		const auto* def = em.UnitArchetype.UnitComponents.GetComponent(id).def;
		int order = em.RenderArchetype.DestinationComponents.GetComponent(id).order;

		Vector2Int16 dst = em.PositionComponents.GetComponent(id);
	
		const auto& selectionImage = def->Art.GetSelectionImage();

		dst += selectionImage.GetImageFrameOffset(0, false);
		dst.y += def->Art.SelectionOffset;

		dst -= camRect.position;
		dst /= camera.Scale;

		cmd.order = order + 1;
		cmd.sprite = &selectionImage.GetFrame(0);
		cmd.position = dst;
		cmd.color = unitSelectionData.color[i];

		render.push_back(cmd);
	}
}

void RenderSystem::Draw(const Camera& camera, EntityManager& em) {
	Rectangle16 camRect = camera.GetRectangle16();

	SectionProfiler p("EntityDraw");

	CameraCull(camRect, em);

	render.clear();

	DrawEntities(camera, camRect, em);
	DrawSelection(camera, camRect, em);

	std::sort(render.begin(), render.end(), RenderSort);

	GraphicsRenderer::BufferDraw(render);
}
void RenderSystem::DrawBoundingBoxes(const Camera& camera, EntityManager& em) {


	Rectangle16 camRect = camera.GetRectangle16();

	Color c = Colors::UIGreen;
	c.a = 0.5f;

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
		p.dst = data.worldPos[i] + data.offset[i].offset;
		p.shadowDst = data.worldPos[i] + data.offset[i].shadowOffset;
		p.order = data.depth[i] * 10'000'000 + p.dst.y * 1000 + p.dst.x * 4;

		data.outBB[i]->SetCenter(data.worldPos[i]);
	}
}

void RenderSystem::UpdatePositions(EntityManager& em, const EntityChangedData& changed) {
	updatePosData.clear();

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

	s = this;

	JobSystem::RunJob(updatePosData.size(), JobSystem::DefaultJobSize, UpdateRenderPositionsJob);
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

