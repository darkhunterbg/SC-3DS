#pragma once

#include "Entity.h"
#include "../Data/AssetDataDefs.h"
#include "../Data/AbilityDef.h"
#include "PlayerSystem.h"
#include "UnitAIStateMachine.h"
#include "UnitStateMachine.h"

#include <vector>

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
	static EntityId SpawnSprite(const SpriteDef& def, Vector2Int16 position, int orientation = 0);

	static bool IsAlly(PlayerId player, EntityId id);
	static bool IsEnemy(PlayerId player, EntityId id);

	static uint8_t GetOrientationToPosition(EntityId id, Vector2Int16 target);
	static void SetUnitAIState(EntityId id, UnitAIStateId state);
	static void SetUnitState(EntityId id, UnitStateId state);

	static const std::vector<const AbilityDef*>& GetUnitAbilities(EntityId id);

	static void ActivateAbility(EntityId user, const AbilityDef& ability);
	static void ActivateAbility(EntityId user, const AbilityDef& ability, EntityId target);
	static void ActivateAbility(EntityId user, const AbilityDef& ability, Vector2Int16 target);

	static  const AbilityDef* GetUnitDefaultAbility(EntityId id, EntityId target);
	static  const AbilityDef* GetUnitDefaultAbility(EntityId id, Vector2Int16 target);
};

