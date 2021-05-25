#include "UnitAIStateMachine.h"
#include "EntityUtil.h"
#include "EntityManager.h"


std::vector<IUnitAIState*> UnitAIStateMachine::States = {
	new UnitAIIdleState(), new UnitAIAttackTargetState(), new UnitAIGoToState(), new UnitAIGoToAttackState(),
};

static std::vector<EntityId> scratch;

static EntityId DetectNearbyEnemy(EntityId id, Vector2Int16 pos, PlayerId owner, EntityManager& em) {

	scratch.clear();

	const auto& unitData = em.UnitArchetype.DataComponents.GetComponent(id);

	short range = (int)(unitData.visiion) << 5;
	Circle16 accquisitonRage = { pos , range };

	em.GetKinematicSystem().CircleCast(accquisitonRage, scratch);

	EntityId result = Entity::None;

	if (scratch.size() > 0)
	{
		int nearest = 0xFFFFFFFF / 2;


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
	}

	return result;
}
static void MoveToPosition(EntityId id, Vector2Int16 pos, Vector2Int16 target, EntityManager& em) {
	Vector2Int16 distance = target - pos;

	if (distance.LengthSquaredInt() < 32) {
		UnitEntityUtil::SetAIState(id, UnitAIState::Idle);
	}
	else if (distance.LengthSquaredInt() < 128) {

		scratch.clear();
		em.GetKinematicSystem().RectCast({ target, {32,32} }, scratch);
		if (scratch.size()) {
			UnitEntityUtil::SetAIState(id, UnitAIState::Idle);
		}
	}
	else {
		em.FlagComponents.GetComponent(id).set(ComponentFlags::NavigationWork);
		em.NavigationArchetype.NavigationComponents.GetComponent(id).target = target;
	}
}

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
		const auto& pos = data.position[i];
		const auto& owner = data.owner[i];

		EntityId enemy = DetectNearbyEnemy(id, pos, owner, em);
		if (enemy != Entity::None) {
			Vector2Int16 trg = em.PositionComponents.GetComponent(id);
			UnitEntityUtil::SetAIState(id, UnitAIState::GoToAttack, trg);
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

		MoveToPosition(id, pos, stateData.target.position, em);
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

		EntityId target = DetectNearbyEnemy(id, pos, data.owner[i], em);

		if (target != Entity::None) {
			//Vector2Int16 trg = em.PositionComponents.GetComponent(id);
			UnitEntityUtil::SetAIState(id, UnitAIState::AttackTarget, target);
			continue;
		}

		MoveToPosition(id, pos, stateData.target.position, em);
	}
}
