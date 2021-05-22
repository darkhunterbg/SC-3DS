#pragma once

#include "Component.h"

class EntityManager;

class UnitSystem {

private:
	
	std::vector<EntityId> scratch;

public:
	void ApplyUnitState(EntityManager& em);
};