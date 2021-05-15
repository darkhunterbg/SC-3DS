#pragma once

#include "Component.h"
#include "Entity.h"
#include "../Span.h"
#include <vector>
#include <array>
#include "../Job.h"
#include "EntityTree.h"


class EntityManager;
struct EntityChangedData;
class Camera;

class KinematicSystem {

	struct UpdateColliderPosData {
		std::vector<EntityId> entity;
		std::vector<Rectangle16> collider;
		std::vector<Vector2Int16> position;
		std::vector<Rectangle16> worldCollider;

		inline size_t size() const { return entity.size(); }
		inline void clear() {
			entity.clear();
			collider.clear();
			position.clear();
			worldCollider.clear();

		}
	};

private:
	ThreadLocal<std::vector< EntityId>> list;
	std::vector< Rectangle16> drawColliders;
	EntityTree collidersTree;
	std::array<bool, Entity::MaxEntities> entityInTree;
	std::array<EntityTreeCellId, Entity::MaxEntities> updateCollider;

	UpdateColliderPosData updateColliderPosData;

	static void ColliderChangeJob(int start, int end);
public:

	KinematicSystem();

	void SetSize(Vector2Int16 size);

	void UpdateCollidersPosition(EntityManager& em, const EntityChangedData& data);

	void ApplyCollidersChange(EntityManager& em);

	void DrawColliders(const Camera& camera);

	inline bool CollidesWithAny(const Rectangle16& collider, EntityId skip) const {
		return collidersTree.CollidesWithAny(collider, skip);
	}
	
	inline void RectCast(const Rectangle16& collider, std::vector<EntityId>& result) const {
		return collidersTree.RectCastEntity(collider, result);
	}

	inline EntityId PointCast(Vector2Int16 point) const {
		return collidersTree.PointCastEntity(point);
	}

	void MoveEntities(EntityManager& em);
};