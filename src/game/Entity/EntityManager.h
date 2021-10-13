#pragma once

#include <vector>
#include <array>

#include "Entity.h"
#include "../Span.h"

#include "../Random.h"

#include "../Data/UnitDef.h"
#include "../Camera.h"

#include "ObjectDrawSystem.h"

struct EntityChangedData {
	std::vector<EntityId> entity;
	std::vector<Vector2Int16> position;
	std::vector<Vector2Int16> oldPosition;

	size_t size() const { return entity.size(); }
	void clear() { entity.clear(); position.clear(); oldPosition.clear(); }
};

class EntityManager {
private:
	EntityManagerCollection entities;

	bool ready = false;

	EntityChangedData changedData;

	std::vector<EntityId> scratch;
	std::vector<EntityId> toDelete;
public:
	Random rand;
private:
	void CollectEntityChanges();
	void ApplyEntityChanges();
	void UpdateChildren();

	void Update0();
	void Update1();
	void Update2();
	void Draw0(const Camera& camera);
	void Draw1(const Camera& camera);
	void Draw2(const Camera& camera);

	unsigned long long frameCounter = 0;
	int8_t updateId = 0;
	uint32_t logicalFrame = 0;

	std::array<Vector2Int16, Entity::MaxEntities> _positions;
public:
	bool DrawColliders = false;
	bool DrawGrid = false;
	bool DrawBoundingBoxes = false;
	bool Muted = false;

	ObjectDrawSystem DrawSystem;

	EntityManager();
	~EntityManager();

	void Init(Vector2Int16 mapSize);

	inline const Span<EntityId> GetEntities() const {
		return entities.GetEntities();
	}
	inline Vector2Int16& GetPosition(EntityId id) { return _positions[Entity::ToIndex(id)]; }

	void DeleteEntity(EntityId id);
	void DeleteEntities(const std::vector<EntityId>& entities, bool sorted = false);
	// NOTE: this will reuse given vector as scratch for performance reasons
	void DeleteEntitiesSorted(std::vector<EntityId>& entities);
	EntityId NewEntity() { return entities.NewEntity(); }
	void NewEntities(unsigned size, std::vector<EntityId>& outIds) {
		entities.NewEntities(size, outIds);
	}
	void ClearEntities();

	void FrameUpdate(const Camera& camera);

	void Draw(const Camera& camera);

	void FullUpdate();
};