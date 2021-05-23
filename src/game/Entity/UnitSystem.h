#pragma once

#include "Component.h"

class EntityManager;

class UnitSystem {

private:
	
	std::vector<EntityId> stateChanged;

public:
	void ApplyUnitState(EntityManager& em);
};