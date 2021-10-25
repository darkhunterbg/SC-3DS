#pragma once

#include <vector>
#include <array>

#include "Entity.h"
#include "../Span.h"

#include "../Random.h"

#include "../Data/UnitDef.h"
#include "../Camera.h"

#include "ObjectDrawSystem.h"
#include "AnimationSystem.h"
#include "PlayerSystem.h"
#include "MapSystem.h"
#include "UnitSystem.h"
#include "KinematicSystem.h"

#include "IEntitySystem.h"


class EntityManager {
private:
	EntityManagerCollection entities;

	bool ready = false;

	std::vector<EntityId> scratch;
	std::vector<EntityId> toDelete;

	std::vector<IEntitySystem*> _systems;
public:
	Random rand;
private:
	void ApplyEntityChanges();

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
	std::array<uint8_t, Entity::MaxEntities> _orientations;
public:

	bool DrawGrid = false;
	bool DrawBoundingBoxes = false;
	bool Muted = false;

	ObjectDrawSystem DrawSystem;
	AnimationSystem AnimationSystem;
	PlayerSystem PlayerSystem;
	MapSystem MapSystem;
	UnitSystem UnitSystem;
	KinematicSystem KinematicSystem;

	EntityManager();
	~EntityManager();

	void Init(Vector2Int16 mapSize);

	inline const auto GetEntities() const
	{
		return entities.GetEntities();
	}
	void SetPosition(EntityId id, Vector2Int16 pos) { _positions[Entity::ToIndex(id)] = pos; }
	void SetOrientation(EntityId id, uint8_t orien) { _orientations[Entity::ToIndex(id)] = orien; }
	inline Vector2Int16 GetPosition(EntityId id) const { return _positions[Entity::ToIndex(id)]; }
	inline uint8_t GetOrientation(EntityId id) const { return _orientations[Entity::ToIndex(id)]; }

	void DeleteEntity(EntityId id);
	void DeleteEntities(const std::vector<EntityId>& entities, bool sorted = false);
	// NOTE: this will reuse given vector as scratch for performance reasons
	void DeleteEntitiesSorted(std::vector<EntityId>& entities);
	EntityId NewEntity() { return entities.NewEntity(); }
	void NewEntities(unsigned size, std::vector<EntityId>& outIds)
	{
		entities.NewEntities(size, outIds);
	}
	void ClearEntities();

	void FrameUpdate(const Camera& camera);

	void Draw(const Camera& camera);

	void FullUpdate();

	size_t GetMemoryUsage();
};