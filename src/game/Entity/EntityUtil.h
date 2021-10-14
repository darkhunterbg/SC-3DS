#pragma once

#include "Entity.h"
#include "../Data/AssetDataDefs.h"
#include "PlayerSystem.h"

class EntityManager;

class EntityUtil {
private:
	EntityUtil() = delete;
	~EntityUtil() = delete;

	static EntityManager* emInstance;

	friend class EntityManager;

public:
	static inline EntityManager& GetManager() { return *emInstance; }

	static void SetOrientation(EntityId e, int orientation);
	static void SetAnimationFrame(EntityId id, unsigned frame);
	static void UpdateAnimationVisual(EntityId id);
	static void PlayAnimation(EntityId id, const AnimClipDef& clip, const Image* shadow = nullptr);

	static EntityId SpawnUnit(const UnitDef& def, PlayerId owner, Vector2Int16 position, int orientation = 0);
};

