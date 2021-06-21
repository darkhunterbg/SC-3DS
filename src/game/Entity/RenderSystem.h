#pragma once

#include <vector>
#include "../Camera.h"
#include "Entity.h"
#include "Component.h"
#include "../Assets.h"
#include "../Span.h"
#include "../Color.h"
#include "../Engine/GraphicsPrimitives.h"

class EntityManager;
class Camera;
struct EntityChangedData;

class RenderSystem {

	struct RenderData {
		std::vector<EntityId> entities;

		inline void clear() {
			entities.clear();
		}
		inline size_t size() const {
			return entities.size();
		}
	};

	struct RenderUpdatePosData {
		std::vector<RenderDestinationComponent*> outDst;
		std::vector<Rectangle16*> outBB;
		std::vector<Vector2Int16> worldPos;
		std::vector<RenderOffsetComponent> offset;
		std::vector<int8_t> depth;

		inline void clear() {

			outDst.clear();
			worldPos.clear();
			offset.clear();
			outBB.clear();
			depth.clear();
		}
		inline size_t size() {
			return outDst.size();
		}
	};

	struct RenderUnitSelectionData {
		std::vector<EntityId> entities;
		std::vector<Color32> color;

		inline void clear() {
			entities.clear();
			color.clear();
		}
		inline size_t size() {
			return entities.size();
		}
	};

private:
	std::vector<BatchDrawCommand> render;
	std::vector<EntityId> selectedUnits;
	std::vector<Color32> selectionColor;

	RenderData renderData;
	RenderUpdatePosData updatePosData;
	RenderUnitSelectionData unitSelectionData;

	static void UpdateRenderPositionsJob(int start, int end);
	static bool RenderSort(const BatchDrawCommand& a, const BatchDrawCommand& b);

	void CameraCull(const Rectangle16& rect, EntityManager& em);
	void UnitSelectionCameraCull(const Rectangle16& rect, EntityManager& em);

	void DrawEntities(const Camera& camera, const Rectangle16& camRect, EntityManager& em);
	void DrawSelection(const Camera& camera, const Rectangle16& camRect, EntityManager& em);
public:
	void Draw(const Camera& camera, EntityManager& em);
	void DrawBoundingBoxes(const Camera& camera, EntityManager& em);
	void UpdatePositions(EntityManager& em, const EntityChangedData& changed);


	void ClearSelection();
	void AddSelection(const std::vector<EntityId>& selection, Color selectionColor);
	void AddSelection(EntityId id, Color selectionColor);
};