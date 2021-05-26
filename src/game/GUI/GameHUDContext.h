#pragma once

#include "../Entity/Entity.h"
#include "../Assets.h"
#include "../Entity/Common.h"
#include "../Data/RaceDef.h"
#include "../Entity/Component.h"
#include "../Data/AbilityDef.h"

class EntityManager;

struct GameHUDContext {

public:
	EntityManager* em = nullptr;
	const RaceDef* race = nullptr;

	EntityCollection selectedEntities;

	PlayerId player = 1;

	bool IsTargetSelectionMode = false;
	const AbilityDef* currentAbility = nullptr;

	void ActivateCurrentAbility();
	void ActivateCurrentAbility(EntityId target);
	void ActivateCurrentAbility(Vector2Int16 position);
	
	void SelectAbilityTarget(const AbilityDef& def);
	void CancelTargetSelection();

	inline EntityManager& GetEntityManager() const {
		return *em;
	}

	inline const SpriteFrameAtlas& GetCommandIconsAtlas() const {
		return *race->CommandIconsAtlas;
	}
};