#pragma once

#include "Component.h"

class EntityManager;

struct UnitStateMachinChangeData {
	std::vector<EntityId> entities;

	inline void clear() { entities.clear(); }
	inline size_t size() const { return entities.size(); }
};

class IUnitState {
public:
	virtual void EnterState(UnitStateMachinChangeData& data, EntityManager& em) = 0;
	virtual void ExitState(UnitStateMachinChangeData& data, EntityManager& em) = 0;
};

class UnitStateMachine {

private:
	UnitStateMachine() = delete;
	~UnitStateMachine() = delete;
public:
	static std::array<IUnitState*, UnitStatesCount> States;
};


class UnitIdleState : public  IUnitState {
public:
	virtual void EnterState(UnitStateMachinChangeData& data, EntityManager& em) override;
	virtual void ExitState(UnitStateMachinChangeData& data, EntityManager& em) override;
};

class UnitTurningState : public  IUnitState {
public:
	virtual void EnterState(UnitStateMachinChangeData& data, EntityManager& em) override;
	virtual void ExitState(UnitStateMachinChangeData& data, EntityManager& em) override;
};

class UnitMovingState : public  IUnitState {
public:
	virtual void EnterState(UnitStateMachinChangeData& data, EntityManager& em) override;
	virtual void ExitState(UnitStateMachinChangeData& data, EntityManager& em) override;
};

class UnitAttackingState : public  IUnitState {
public:
	virtual void EnterState(UnitStateMachinChangeData& data, EntityManager& em) override;
	virtual void ExitState(UnitStateMachinChangeData& data,  EntityManager& em) override;
};

class UnitDeathState : public  IUnitState {
public:
	virtual void EnterState(UnitStateMachinChangeData& data, EntityManager& em) override;
	virtual void ExitState(UnitStateMachinChangeData& data, EntityManager& em) override;
};
