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

struct Collider {
	Rectangle16 rect;
	EntityId id;
};

class KinematicSystem {
private:
	ThreadLocal<std::vector< EntityId>> list;
	std::vector< Rectangle16> drawColliders;
	EntityTree collidersTree;
	std::array<bool, Entity::MaxEntities> entityInTree;
	std::array<EntityTreeCellId, Entity::MaxEntities> updateCollider;
	static void ColliderChangeJob(int start, int end);
public:

	KinematicSystem();

	void SetSize(Vector2Int16 size);

	void UpdateCollidersPosition( EntityManager& em, const EntityChangedData& data);

	void ApplyCollidersChange( EntityManager& em, const EntityChangedData& data);

	void DrawColliders(const Camera& camera);

	bool CollidesWithAny(const Rectangle16& rect, EntityId skip);
};