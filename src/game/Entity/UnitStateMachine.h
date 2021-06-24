#pragma once

#include "Component.h"

class EntityManager;

struct UnitStateMachineChangeData {
	std::vector<EntityId> entities;

	inline void clear() { entities.clear(); }
	inline size_t size() const { return entities.size(); }
};

class IUnitState {
public:
	virtual void EnterState(UnitStateMachineChangeData& data, EntityManager& em) = 0;
	virtual void ExitState(UnitStateMachineChangeData& data, EntityManager& em) = 0;
};

class UnitStateMachine {

private:
	UnitStateMachine() = delete;
	~UnitStateMachine() = delete;
public:
	static std::vector<IUnitState*> States;
};


class UnitIdleState : public  IUnitState {
public:
	virtual void EnterState(UnitStateMachineChangeData& data, EntityManager& em) override;
	virtual void ExitState(UnitStateMachineChangeData& data, EntityManager& em) override;
};

class UnitTurningState : public  IUnitState {
public:
	virtual void EnterState(UnitStateMachineChangeData& data, EntityManager& em) override;
	virtual void ExitState(UnitStateMachineChangeData& data, EntityManager& em) override;
};

class UnitMovingState : public  IUnitState {
public:
	virtual void EnterState(UnitStateMachineChangeData& data, EntityManager& em) override;
	virtual void ExitState(UnitStateMachineChangeData& data, EntityManager& em) override;
};

class UnitAttackingState : public  IUnitState {
public:
	virtual void EnterState(UnitStateMachineChangeData& data, EntityManager& em) override;
	virtual void ExitState(UnitStateMachineChangeData& data,  EntityManager& em) override;
};

class UnitDeathState : public  IUnitState {
public:
	virtual void EnterState(UnitStateMachineChangeData& data, EntityManager& em) override;
	virtual void ExitState(UnitStateMachineChangeData& data, EntityManager& em) override;
};


class UnitProducingState : public IUnitState {
public:
	virtual void EnterState(UnitStateMachineChangeData& data, EntityManager& em) override;
	virtual void ExitState(UnitStateMachineChangeData& data, EntityManager& em) override;
};


class UnitMiningState : public IUnitState {
public:
	virtual void EnterState(UnitStateMachineChangeData& data, EntityManager& em) override;
	virtual void ExitState(UnitStateMachineChangeData& data, EntityManager& em) override;
};

class UnitAttackLoopState : public IUnitState {
	virtual void EnterState(UnitStateMachineChangeData& data, EntityManager& em) override;
	virtual void ExitState(UnitStateMachineChangeData& data, EntityManager& em) override;
};