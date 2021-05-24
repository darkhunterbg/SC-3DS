#pragma once

#include "Component.h"
#include <vector>

class EntityManager;

struct UnitAIStateMachineData {
	std::vector<EntityId> entities;
	std::vector<UnitAIStateDataComponent> stateData;
	std::vector<Vector2Int16> position;
	std::vector<PlayerId> owner;

	inline void clear() {
		entities.clear();
		stateData.clear();
		position.clear();
		owner.clear();
	}
	inline size_t size() const { return entities.size(); }
};

class IUnitAIState {
public:
	virtual void Think(UnitAIStateMachineData& data, EntityManager& em) = 0;
};


class UnitAIStateMachine {

private:
	UnitAIStateMachine() = delete;
	~UnitAIStateMachine() = delete;
public:
	static std::vector<IUnitAIState*> States;
};


class UnitAIIdleState : public  IUnitAIState {
public:
	virtual void Think(UnitAIStateMachineData& data, EntityManager& em) override;
};


class UnitAIAttackTargetState : public  IUnitAIState {
public:
	virtual void Think(UnitAIStateMachineData& data, EntityManager& em) override;
};


class UnitAIGoToState : public  IUnitAIState {
public:
	virtual void Think(UnitAIStateMachineData& data, EntityManager& em) override;
};


class UnitAIGoToAttackState : public  IUnitAIState {
public:
	virtual void Think(UnitAIStateMachineData& data, EntityManager& em) override;
};
