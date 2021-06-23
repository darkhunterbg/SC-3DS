#pragma once

#include "../UnitStateMachine.h"


class UnitIdleState : public  IUnitState {
public:
	virtual void EnterState(UnitStateMachineChangeData& data, EntityManager& em) override;
	virtual void ExitState(UnitStateMachineChangeData& data, EntityManager& em) override;
};