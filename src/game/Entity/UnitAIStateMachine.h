#pragma once

#include <vector>
#include <cstdint>

#include "Entity.h"

class EntityManager;


struct UnitAIThinkData {
	std::vector<EntityId> entities;

	int start = 0;
	int end = 0;

	inline void clear()
	{
		entities.clear();
	}
	inline size_t size() const { return entities.size(); }
};

using UnitAIThinkFunc = void(*)(UnitAIThinkData& data, EntityManager& em);

enum class UnitAIStateId :uint8_t {

	Idle = 0,
	AttackTarget = 1,
	GoTo = 2,
	GoToAttack = 3,
	Patrol = 4,
	HoldPosition = 5,

	Nothing = 0xFF
};

static constexpr const int UnitAIStateTypeCount = 6;

struct UnitAIState {
public:
	UnitAIThinkData thinkData;

	UnitAIThinkFunc  thinkFunc = nullptr;

	inline size_t GetSize() const
	{
		size_t  size = thinkData.size() * sizeof(EntityId);

		return size;
	}

	UnitAIState( UnitAIThinkFunc think) :
		thinkFunc(think)
	{
	}

	void Start(int batch);
	void Advance(int batch);

	inline bool IsCompleted() { return thinkData.end >= thinkData.size(); }
};

class UnitAIStateMachine {
private:
	UnitAIStateMachine() = delete;
	~UnitAIStateMachine() = delete;
public:
	static void CreateStates(std::vector< UnitAIState*>& states);

private:
	static void IdleAggresiveThink(UnitAIThinkData& data, EntityManager& em);
	static void AttackTargetThink(UnitAIThinkData& data, EntityManager& em);
	static void GoToThink(UnitAIThinkData& data, EntityManager& em);
	static void GoToAttackThink(UnitAIThinkData& data, EntityManager& em);
	static void PatrolThink(UnitAIThinkData& data, EntityManager& em);
	static void HoldPositionThink(UnitAIThinkData& data, EntityManager& em);
};

