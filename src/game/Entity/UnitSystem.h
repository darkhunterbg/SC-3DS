#pragma once

#include "Component.h"
#include "UnitStateMachine.h"
#include <vector>
#include <array>

class EntityManager;

class UnitSystem {
private:
	std::array<UnitStateMachinChangeData, UnitStatesCount> exitStateData;
	std::array<UnitStateMachinChangeData, UnitStatesCount> enterStateData;
public:
	void ApplyUnitState(EntityManager& em);
};