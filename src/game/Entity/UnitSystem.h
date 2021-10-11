#pragma once

#include "Component.h"
#include "UnitStateMachine.h"
#include "UnitAIStateMachine.h"
#include <vector>
#include <array>

template <class T>
class Node {
	T data;
	Node* prev, * next;
};

template <class T>
class List {
	Node<T>* start, * end;
};

class Data {
	Data* next, * prev;
	/// 
};

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

	std::vector<EntityId> attackUnits;

	 void ResolveUnitEvents(EntityManager& em);
public:
	UnitSystem();

	void UnitAIUpdate(EntityManager& em);
	void UpdateUnitCooldowns(EntityManager& em);
	void ApplyUnitState(EntityManager& em);
	void UpdateUnitStats(EntityManager& em);

	void UpdateBuilding(EntityManager& em);

	void UnitAttackEvent(EntityId id);

	void DequeueItem(EntityId id, int itemId, EntityManager& em);

	inline void EnqueueBuildUpdateCheck(EntityId id, const UnitDef& def, bool paid = false)
	{
		unitBuildUpdate.push_back({ &def, id, paid });
	}
};