#pragma once

#include "Entity.h"
#include "../Data/AssetDataDefs.h"

class EntityManager;

class EntityUtil {
private:
	EntityUtil() = delete;
	~EntityUtil() = delete;

	static EntityManager* emInstance;

	friend class EntityManager;

	static inline  EntityManager& GetManager() { return *emInstance; }


public:
	static void SetOrientation(EntityId e, int orientation);
	static void SetAnimationFrame(EntityId id, unsigned frame);
	static void UpdateAnimationVisual(EntityId id);
	static void PlayAnimation(EntityId id, const AnimClipDef& clip, const Image* shadow = nullptr);
};