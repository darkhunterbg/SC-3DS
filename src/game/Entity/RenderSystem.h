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
		std::vector<FlagsComponent> flags;
		std::vector<RenderShadowComponent> shadow;

		inline void clear() {
			pos.clear();
			ren.clear();
			flags.clear();
			shadow.clear();
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
		std::vector<Vector2Int16> shadowOffset;
		std::vector<int8_t> depth;

		inline void clear() {

			outDst.clear();
			worldPos.clear();
			offset.clear();
			shadowOffset.clear();
			outBB.clear();
			depth.clear();
		}
		inline size_t size() {
			return outDst.size();
		}
	};

	struct RenderUnitSelectionData {
		std::vector<ImageFrame> graphics;
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
	RenderUpdatePosData updatePosData;
	RenderUnitSelectionData unitSelectionData;

	static void UpdateRenderPositionsJob(int start, int end);
	static bool RenderSort(const BatchDrawCommand& a, const BatchDrawCommand& b);

	void CameraCull(const Rectangle16& rect, EntityManager& em);
	void UnitSelectionCameraCull(const Rectangle16& rect, EntityManager& em);

	void DrawEntities(const Camera& camera, const Rectangle16& camRect);
	void DrawSelection(const Camera& camera, const Rectangle16& camRect);
public:
	void Draw(const Camera& camera, EntityManager& em);
	void DrawBoundingBoxes(const Camera& camera, EntityManager& em);
	void UpdatePositions(EntityManager& em, const EntityChangedData& changed);


	void ClearSelection();
	void AddSelection(const std::vector<EntityId>& selection, Color selectionColor);
	void AddSelection(EntityId id, Color selectionColor);
};