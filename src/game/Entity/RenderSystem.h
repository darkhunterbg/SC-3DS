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
		std::vector<RenderDestinationComponent> pos;
		std::vector<RenderComponent> ren;

		inline void clear() {
			pos.clear();
			ren.clear();
		}
		inline size_t size() const {
			return pos.size();
		}
	};

	struct RenderUnitData {
		std::vector<RenderUnitDestinationComponent> pos;
		std::vector<RenderUnitComponent> ren;

		inline void clear() {
			pos.clear();
			ren.clear();
		}
		inline size_t size() const {
			return pos.size();
		}
	};

	struct RenderUpdatePosData {
		std::vector<RenderDestinationComponent*> outDst;
		std::vector<Rectangle16*> outBB;
		std::vector<Vector2Int16> worldPos;
		std::vector<Vector2Int16> offset;
		std::vector< int8_t> depth;

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

	struct RenderUnitUpdatePosData {
		std::vector<RenderUnitDestinationComponent*> outPos;
		std::vector<Rectangle16*> outBB;
		std::vector<Vector2Int16> worldPos;
		std::vector<RenderUnitOffsetComponent> offset;
		std::vector<int8_t> depth;

		inline void clear() {
			outPos.clear();
			worldPos.clear();
			offset.clear();
			outBB.clear();
			depth.capacity();
		}
		inline size_t size() {
			return outPos.size();
		}
	};

	struct RenderUnitSelectionData {
		std::vector<SpriteFrame> graphics;
		std::vector<Vector2Int16> position;
		std::vector<int> order;
		std::vector<short> verticalOffset;
		std::vector<Color32> color;

		inline void clear() {
			graphics.clear();
			position.clear();
			order.clear();
			verticalOffset.clear();
			color.clear();
		}
		inline size_t size() {
			return graphics.size();
		}
	};

private:
	std::vector<BatchDrawCommand> render;
	std::vector<EntityId> selectedUnits;
	std::vector<Color32> selectionColor;

	RenderData renderData;
	RenderUnitData renderUnitData;
	RenderUpdatePosData updatePosData;
	RenderUnitUpdatePosData unitUpdatePosData;
	RenderUnitSelectionData unitSelectionData;

	static void UpdateUnitRenderPositionsJob(int start, int end);
	static void UpdateRenderPositionsJob(int start, int end);
	static bool RenderSort(const BatchDrawCommand& a, const BatchDrawCommand& b);

	void CameraCull(const Rectangle16& rect, EntityManager& em);
	void UnitSelectionCameraCull(const Rectangle16& rect, EntityManager& em);

	void DrawEntities(const Camera& camera, const Rectangle16& camRect);
	void DrawUnits(const Camera& camera, const Rectangle16& camRect);
	void DrawSelection(const Camera& camera, const Rectangle16& camRect);
public:
	void Draw(const Camera& camera, EntityManager& em);
	void DrawBoundingBoxes(const Camera& camera, EntityManager& em);
	void UpdatePositions(EntityManager& em, const EntityChangedData& changed);


	void ClearSelection();
	void AddSelection(const std::vector<EntityId>& selection, Color selectionColor);
	void AddSelection(EntityId id, Color selectionColor);
};