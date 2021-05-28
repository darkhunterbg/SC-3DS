#pragma once

#include "Component.h"
#include "UnitStateMachine.h"
#include "UnitAIStateMachine.h"
#include <vector>
#include <array>

class EntityManager;

class UnitSystem {
	struct BuildUpdateData {
		const UnitDef* def = nullptr;
		EntityId id;
		bool paid;
	};

private:
	std::vector<UnitStateMachineChangeData> exitStateData;
	std::vector<UnitStateMachineChangeData> enterStateData;


	std::vector<UnitAIEnterStateData> aiEnterStateData;
	std::vector<UnitAIThinkData> aiThinkData;


	std::vector<BuildUpdateData> unitBuildUpdate;

public:
	UnitSystem();

	void UnitAIUpdate(EntityManager& em);
	void UpdateUnitCooldowns(EntityManager& em);
	void ApplyUnitState(EntityManager& em);
	void UpdateUnitStats(EntityManager& em);

	void UpdateBuilding(EntityManager& em);

	void DequeueItem(EntityId id, int itemId, EntityManager& em);

	inline void EnqueueBuildUpdateCheck(EntityId id, const UnitDef& def, bool paid = false)
	{
		unitBuildUpdate.push_back({ &def, id, paid });
	}
};