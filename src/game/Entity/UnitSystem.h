#pragma once

#include "Component.h"
#include "UnitStateMachine.h"
#include "UnitAIStateMachine.h"
#include <vector>
#include <array>

class EntityManager;

class UnitSystem {
private:
	std::vector<UnitStateMachineChangeData> exitStateData;
	std::vector<UnitStateMachineChangeData> enterStateData;


	std::vector<UnitAIEnterStateData> aiEnterStateData;
	std::vector<UnitAIThinkData> aiThinkData;

public:
	UnitSystem();

	void UnitAIUpdate(EntityManager& em);
	void UpdateUnitCooldowns(EntityManager& em);
	void ApplyUnitState(EntityManager& em);
	void UpdateUnitStats(EntityManager& em);
};