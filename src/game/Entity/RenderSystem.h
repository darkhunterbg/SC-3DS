#pragma once

#include <vector>
#include "../Camera.h"
#include "Entity.h"
#include "Component.h"
#include "../Assets.h"
#include "../Span.h"
#include "../Color.h"
#include "../Platform.h"

class EntityManager;
class Camera;
struct EntityChangedData;

class RenderSystem {

	struct RenderData {
		std::vector<Vector2Int16> pos;
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
		std::vector<Vector2Int16*> outPos;
		std::vector<Rectangle16*> outBB;
		std::vector<Vector2Int16> worldPos;
		std::vector<Vector2Int16> offset;

		inline void clear() {

			outPos.clear();
			worldPos.clear();
			offset.clear();
			outBB.clear();
		}
		inline size_t size() {
			return outPos.size();
		}
	};

	struct RenderUnitUpdatePosData {
		std::vector<RenderUnitDestinationComponent*> outPos;
		std::vector<Rectangle16*> outBB;
		std::vector<Vector2Int16> worldPos;
		std::vector<RenderUnitOffsetComponent> offset;

		inline void clear() {
			outPos.clear();
			worldPos.clear();
			offset.clear();
			outBB.clear();
		}
		inline size_t size() {
			return outPos.size();
		}
	};

private:
	std::vector< BatchDrawCommand> render;

	RenderData renderData;
	RenderUnitData renderUnitData;
	RenderUpdatePosData renderUpdatePosData;
	RenderUnitUpdatePosData renderUnitUpdatePosData;

	static void UpdateUnitRenderPositionsJob(int start, int end);
	static void UpdateRenderPositionsJob(int start, int end);
	static bool RenderSort(const BatchDrawCommand& a, const BatchDrawCommand& b);

	void CameraCull(const Rectangle16& rect, EntityManager& em);

	void DrawEntities(const Camera& camera, const Rectangle16& camRect);
	void DrawUnits(const Camera& camera, const Rectangle16& camRect);
public:
	void Draw(const Camera& camera, EntityManager& em);
	void UpdatePositions(EntityManager& em, const EntityChangedData& changed);

};