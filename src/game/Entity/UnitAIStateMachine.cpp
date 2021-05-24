#include "UnitAIStateMachine.h"
#include "EntityUtil.h"
#include "EntityManager.h"


std::vector<IUnitAIState*> UnitAIStateMachine::States = {
	new UnitAIIdleState(), new UnitAIAttackTargetState(), new UnitAIGoToState(), new UnitAIGoToAttackState(),
};


static std::vector<EntityId> scratch;

void UnitAIIdleState::Think(UnitAIStateMachineData& data, EntityManager& em)
{
	int start = 0, end = data.size();
	for (int i = start; i < end; ++i) {
		scratch.clear();

		EntityId id = data.entities[i];
		const auto& stateData = data.stateData[i];
		const auto& pos = data.position[i];
		const auto& owner = data.owner[i];

		const auto& unitData = em.UnitArchetype.DataComponents.GetComponent(id);

		short range = (int)(unitData.visiion) << 5;
		Circle16 accquisitonRage = { pos , range };

		em.GetKinematicSystem().CircleCast(accquisitonRage, scratch);

		if (scratch.size() > 0)
		{
			int nearest = 0xFFFFFFFF / 2;

			EntityId result = Entity::None;

			for (EntityId e : scratch) {
				if (e == id)
					continue;

				if (em.UnitArchetype.OwnerComponents.GetComponent(e) != owner) {

					int t = em.PositionComponents.GetComponent(e).LengthSquaredInt();
					if (t < nearest)
					{
						nearest = t;
						result = e;
					}
				}
			}

			if (result != Entity::None) {
				em.UnitArchetype.AIStateComponents.GetComponent(id) = UnitAIState::AttackTarget;
				em.UnitArchetype.AIStateDataComponents.GetComponent(id).target.entityId = result;
			}
		}
	}
}

void UnitAIAttackTargetState::Think(UnitAIStateMachineData& data, EntityManager& em)
{
	int start = 0, end = data.size();
	for (int i = start; i < end; ++i) {
		EntityId id = data.entities[i];
		const auto& stateData = data.stateData[i];
		const auto& pos = data.position[i];
		const auto& weapon = em.UnitArchetype.WeaponComponents.GetComponent(id);

		EntityId target = stateData.target.entityId;

		if (!em.UnitArchetype.Archetype.HasEntity(target))
		{
			em.UnitArchetype.StateComponents.GetComponent(id) = UnitState::Idle;
			em.UnitArchetype.AIStateComponents.GetComponent(id) = UnitAIState::Idle;
			em.FlagComponents.GetComponent(id).set(ComponentFlags::UnitStateChanged);
			continue;
		}

		Vector2Int16 targetPos = em.PositionComponents.GetComponent(target);

		Vector2Int16 distance = targetPos - pos;

		int range = (int)(weapon.maxRange + 1) << 5;

		if (distance.LengthSquaredInt() <= range * range) {

			if (weapon.IsReady())
				UnitEntityUtil::AttackPosition(id, targetPos);
		}
		else {
			UnitEntityUtil::GoTo(id, targetPos);
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
			em.FlagComponents.GetComponent(id).set(ComponentFlags::NavigationWork);
			em.NavigationArchetype.NavigationComponents.GetComponent(id).target = stateData.target.position;

		}
	}
}

void UnitAIGoToAttackState::Think(UnitAIStateMachineData& data, EntityManager& em)
{
	int start = 0, end = data.size();
	for (int i = start; i < end; ++i) {
		EntityId id = data.entities[i];
		const auto& stateData = data.stateData[i];
		const auto& pos = data.position[i];

		Vector2Int16 distance = stateData.target.position - pos;

		scratch.clear();



		if (distance.LengthSquaredInt() < 32) {
			em.UnitArchetype.StateComponents.GetComponent(id) = UnitState::Idle;
			em.UnitArchetype.AIStateComponents.GetComponent(id) = UnitAIState::Idle;
			em.FlagComponents.GetComponent(id).set(ComponentFlags::UnitStateChanged);
		}
		else {
			em.FlagComponents.GetComponent(id).set(ComponentFlags::NavigationWork);
			em.NavigationArchetype.NavigationComponents.GetComponent(id).target = stateData.target.position;

		}
	}
}
