#pragma once

#include <array>
#include <vector>

#include "Entity.h"
#include "Component.h"

class EntityManager;

class TimingSystem {
	struct ExpiredTimersUpdate {
		std::vector<EntityId> entities;
	};
private:
	std::vector<EntityId> scratch;
	std::array< ExpiredTimersUpdate, TimerExpiredActionTypeCount> actionsTable;

	inline constexpr std::vector<EntityId>& GetActionEntityTable(TimerExpiredAction action) {
		return actionsTable[(int)action].entities;
	}
	void UnitDeathAfterEffect(std::vector<EntityId>& entities, EntityManager& em);
	void DeleteEntities( std::vector<EntityId>& entities , EntityManager& em);
	void WeaponAttack(std::vector<EntityId>& entities, EntityManager& em);
public:
	void UpdateTimers(EntityManager& em);
	void ApplyTimerActions(EntityManager& em);
};