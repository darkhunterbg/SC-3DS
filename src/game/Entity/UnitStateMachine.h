#pragma once

#include "Entity.h"
#include <vector>
#include <stdint.h>


enum class UnitStateId :uint8_t {

	Idle = 0,
	Movement = 1,
	Attack = 2
};

static constexpr const int UnitStateCount = 3;

struct UnitStateData {
	std::vector<EntityId> entities;
	std::vector<UnitStateId> otherState;

	size_t size() const { return entities.size(); }

	int start = 0;
	int end = 0;

	void Start(int batch);
	void Advance(int batch);
	inline bool IsCompleted() { return end >= entities.size(); }
	inline void clear()
	{
		entities.clear();
		otherState.clear();
	}
};

using UnitEnterStateFunc = void(*)(UnitStateData& data, class EntityManager& em);

struct UnitState {
	UnitStateData EnterState;
	UnitStateData ExitState;

	UnitEnterStateFunc enterStateFunc = nullptr;
	UnitEnterStateFunc exitStateFunc = nullptr;


	inline size_t GetSize() const
	{
		size_t size = (EnterState.entities.capacity() + ExitState.entities.capacity()) * sizeof(EntityId);
		return size;
	}


	UnitState(UnitEnterStateFunc enterState, UnitEnterStateFunc exitState) : enterStateFunc(enterState),
		exitStateFunc(exitState)
	{
	}

};



class UnitStateMachine {
private:
	UnitStateMachine() = delete;
	~UnitStateMachine() = delete;
public:

	static void CreateStates(std::vector< UnitState*>& states);

private:
	static void IdleEnter(UnitStateData& data, class EntityManager& em);
	static void MovementEnter(UnitStateData& data, class EntityManager& em);
	static void MovementExit(UnitStateData& data, class EntityManager& em);
	static void AttackEnter(UnitStateData& data, class EntityManager& em);
	static void AttackExit(UnitStateData& data, class EntityManager& em);
};