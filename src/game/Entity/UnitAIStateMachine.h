#pragma once

#include <vector>
#include "Entity.h"

class EntityManager;

struct UnitAIEnterStateData {
	std::vector<EntityId> entities;

	inline void clear()
	{
		entities.clear();
	}
	inline size_t size() const { return entities.size(); }
};

struct UnitAIThinkData {
	std::vector<EntityId> entities;

	inline void clear()
	{
		entities.clear();
	}
	inline size_t size() const { return entities.size(); }
};

typedef void (*UnitAIThinkFunc)(UnitAIThinkData& data, EntityManager& em);
typedef void (*UnitAIEnterStateFunc)(UnitAIEnterStateData& data, EntityManager& em);

enum class UnitAIStateId :uint8_t {

	IdleAggressive = 0,
	IdlePassive = 1,
	AttackTarget = 2,
	AttackLoop = 3,
};

static constexpr const int UnitAIStateTypeCount = 4;

struct UnitAIState {
public:
	UnitAIEnterStateData enterStateData;
	UnitAIThinkData thinkData;

	UnitAIEnterStateFunc enterStateFunc = nullptr;
	UnitAIThinkFunc  thinkFunc = nullptr;

	inline size_t GetSize() const
	{
		size_t  size =  enterStateData.size() * sizeof(EntityId);
		size += thinkData.size() * sizeof(EntityId);

		return size;
	}

	UnitAIState(UnitAIEnterStateFunc enterState, UnitAIThinkFunc think) : enterStateFunc(enterState),
		thinkFunc(think)
	{
	}
};

class UnitAIStateMachine {
private:
	UnitAIStateMachine() = delete;
	~UnitAIStateMachine() = delete;
public:
	static void CreateStates(std::vector< UnitAIState*>& states);

private:
	static void IdleEnter(UnitAIEnterStateData& data, EntityManager& em);
	static void IdleAggresiveThink(UnitAIThinkData& data, EntityManager& em);

	static void AttackTargetThink(UnitAIThinkData& data, EntityManager& em);

	static void AttackLoopEnter(UnitAIEnterStateData& data, EntityManager& em);
	static void AttackLoopThink(UnitAIThinkData& data, EntityManager& em);

};

