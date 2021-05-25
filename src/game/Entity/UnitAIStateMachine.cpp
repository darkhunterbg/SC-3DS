#include "UnitAIStateMachine.h"
#include "EntityUtil.h"
#include "EntityManager.h"


std::vector<IUnitAIState*> UnitAIStateMachine::States = {
	new UnitAIIdleState(), new UnitAIAttackTargetState(), new UnitAIGoToState(), new UnitAIGoToAttackState(),
};

static std::vector<EntityId> scratch;

void UnitAIIdleState::EnterState(UnitAIEnterStateData& data, EntityManager& em)
{
	for (EntityId id : data.entities) {
		em.UnitArchetype.StateComponents.GetComponent(id) = UnitState::Idle;
		em.FlagComponents.GetComponent(id).set(ComponentFlags::UnitStateChanged);
	}
}
void UnitAIIdleState::Think(UnitAIThinkData& data, EntityManager& em)
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

					int t = (em.PositionComponents.GetComponent(e) - pos).LengthSquaredInt();
					if (t < nearest)
					{
						nearest = t;
						result = e;
					}
				}
			}

			if (result != Entity::None) {
				UnitEntityUtil::SetAIState(id, UnitAIState::AttackTarget, result);
			}
		}
	}
}

void UnitAIAttackTargetState::EnterState(UnitAIEnterStateData& data, EntityManager& em)
{
}
void UnitAIAttackTargetState::Think(UnitAIThinkData& data, EntityManager& em)
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
			UnitEntityUtil::SetAIState(id, UnitAIState::Idle);
			continue;
		}

		Vector2Int16 targetPos = em.PositionComponents.GetComponent(target);

		Vector2Int16 distance = targetPos - pos;

		int range = (int)(weapon.maxRange + 1) << 5;

		if (distance.LengthSquaredInt() <= range * range) {

			if (weapon.IsReady())
				UnitEntityUtil::AttackTarget(id, target);
		}
		else {
			UnitEntityUtil::GoTo(id, targetPos);
		}
	}
}

void UnitAIGoToState::EnterState(UnitAIEnterStateData& data, EntityManager& em)
{
}
void UnitAIGoToState::Think(UnitAIThinkData& data, EntityManager& em)
{
	int start = 0, end = data.size();
	for (int i = start; i < end; ++i) {
		EntityId id = data.entities[i];
		const auto& stateData = data.stateData[i];
		const auto& pos = data.position[i];

		Vector2Int16 distance = stateData.target.position - pos;

		if (distance.LengthSquaredInt() < 32) {
			UnitEntityUtil::SetAIState(id, UnitAIState::Idle);
		}
		else if (distance.LengthSquaredInt() < 128) {

			scratch.clear();
			em.GetKinematicSystem().RectCast({ stateData.target.position, {32,32} }, scratch);
			if (scratch.size()) {
				UnitEntityUtil::SetAIState(id, UnitAIState::Idle);
			}
		}
		else{
			em.FlagComponents.GetComponent(id).set(ComponentFlags::NavigationWork);
			em.NavigationArchetype.NavigationComponents.GetComponent(id).target = stateData.target.position;
		}
	}
}

void UnitAIGoToAttackState::EnterState(UnitAIEnterStateData& data, EntityManager& em)
{
}
void UnitAIGoToAttackState::Think(UnitAIThinkData& data, EntityManager& em)
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
