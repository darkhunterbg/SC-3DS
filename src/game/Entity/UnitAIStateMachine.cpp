#include "UnitAIStateMachine.h"
#include "EntityUtil.h"
#include "EntityManager.h"


std::vector<IUnitAIState*> UnitAIStateMachine::States = {
	new UnitAIIdleState(), new UnitAIAttackTargetState(), new UnitAIGoToState()
};

void UnitAIIdleState::Think(UnitAIStateMachineData& data, EntityManager& em)
{

}

void UnitAIAttackTargetState::Think(UnitAIStateMachineData& data, EntityManager& em)
{
	int start = 0, end = data.size();
	for (int i = start; i < end; ++i) {
		EntityId id = data.entities[i];
		const auto& stateData = data.stateData[i];
		const auto& pos = data.position[i];
		const auto& weapon = em.UnitArchetype.WeaponComponents.GetComponent(id);

		Vector2Int16 distance = stateData.target.position - pos;

		int range = (int)(weapon.maxRange + 1) << 5;

		if (distance.LengthSquaredInt() <= range * range) {

			if (weapon.IsReady())
				UnitEntityUtil::AttackPosition(id, stateData.target.position);
		}
		else {
			UnitEntityUtil::GoTo(id, stateData.target.position);
		}
	}
}

void UnitAIGoToState::Think(UnitAIStateMachineData& data, EntityManager& em)
{
	int start = 0, end = data.size();
	for (int i = start; i < end; ++i) {
		EntityId id = data.entities[i];
		const auto& stateData = data.stateData[i];
		const auto& pos = data.position[i];

		Vector2Int16 distance = stateData.target.position - pos;

		if (distance.LengthSquaredInt() < 32) {
			em.UnitArchetype.StateComponents.GetComponent(id) = UnitState::Idle;
			em.UnitArchetype.AIStateComponents.GetComponent(id) = UnitAIState::Idle;
			em.FlagComponents.GetComponent(id).set(ComponentFlags::UnitStateChanged);
		}
		else {
			UnitEntityUtil::GoTo(id, stateData.target.position);
		}
	}
}
