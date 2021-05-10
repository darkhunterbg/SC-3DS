#pragma once

#include <vector>
#include "../Camera.h"
#include "Entity.h"
#include "Component.h"
#include "../Assets.h"
#include "../Span.h"
#include "../Color.h"
#include "../Platform.h"
#include "Archetype.h"

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

	struct RenderUpdatePosData {
		std::vector<RenderDestinationComponent*> outPos;
		std::vector<Rectangle16*> outBB;
		std::vector<Vector2Int16> worldPos;
		std::vector<RenderOffsetComponent> offset;

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
	RenderUpdatePosData renderUpdatePosData;


	void CameraCull(const Rectangle16& rect, EntityManager& em);

	static void UpdateRenderPositionsJob(int start, int end);

	static bool RenderSort(const BatchDrawCommand& a, const BatchDrawCommand& b);
public:
	void Draw(const Camera& camera, EntityManager& em);
	void UpdatePositions(EntityManager& em, const EntityChangedData& changed);

};