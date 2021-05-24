#pragma once

#include "Component.h"
#include "UnitStateMachine.h"
#include <vector>
#include <array>

class EntityManager;

class UnitSystem {
private:
	std::array<UnitStateMachineChangeData, UnitStatesCount> exitStateData;
	std::array<UnitStateMachineChangeData, UnitStatesCount> enterStateData;
public:
	void ApplyUnitState(EntityManager& em);
};