#pragma once

#include "../UnitStateMachine.h"

class UnitAttackingState : public  IUnitState {
public:
	virtual void EnterState(UnitStateMachineChangeData& data, EntityManager& em) override;
	virtual void ExitState(UnitStateMachineChangeData& data, EntityManager& em) override;
};