#pragma once

#include "Component.h"
#include "UnitStateMachine.h"
#include "UnitAIStateMachine.h"
#include <vector>
#include <array>

class EntityManager;

class UnitSystem {
private:
	std::array<UnitStateMachineChangeData, UnitStatesCount> exitStateData;
	std::array<UnitStateMachineChangeData, UnitStatesCount> enterStateData;

	std::vector<UnitAIStateMachineData> aiThinkData;

public:
	UnitSystem();

	void UnitAIUpdate(EntityManager& em);
	void UpdateUnitCooldowns(EntityManager& em);
	void ApplyUnitState(EntityManager& em);
	void UpdateUnitStats(EntityManager& em);
};