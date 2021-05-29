#include "UnitAIStateMachine.h"
#include "EntityUtil.h"
#include "EntityManager.h"


std::vector<IUnitAIState*> UnitAIStateMachine::States = {
	new UnitAIIdleState(), new UnitAIAttackTargetState(), new UnitAIGoToState(), new UnitAIGoToAttackState(),
	new UnitAIHoldPositionState(), new UnitAIPatrolState(), new UnitAIFollowState(), new UnitAIGatherResoureState()
};

static std::vector<EntityId> scratch;

static EntityId DetectNearbyEnemy(EntityId id, Vector2Int16 pos, PlayerId owner, EntityManager& em) {

	scratch.clear();

	const auto& weapon = em.UnitArchetype.WeaponComponents.GetComponent(id);

	short range = (int)(weapon.maxRange + 1) << 5;
	Circle16 accquisitonRage = { pos , range };

	em.GetKinematicSystem().CircleCast(accquisitonRage, scratch);

	EntityId result = Entity::None;

	if (scratch.size() > 0)
	{
		int nearest = 0xFFFFFFFF / 2;

		for (EntityId e : scratch) {
			if (e == id)
				continue;

			PlayerId enemyOwner = em.UnitArchetype.OwnerComponents.GetComponent(e);

			if (enemyOwner && enemyOwner != owner) {

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
static bool TryMoveToPosition(EntityId id, Vector2Int16 pos, Vector2Int16 target, EntityManager& em) {
	Vector2Int16 distance = target - pos;

	if (distance.LengthSquaredInt() < 10) {
		return true;
	}
	/*else if (distance.LengthSquaredInt() < 128) {

		scratch.clear();
		em.GetKinematicSystem().RectCast({ target, {32,32} }, scratch);
		if (scratch.size()) {
			UnitEntityUtil::SetAIState(id, UnitAIState::Idle);
		}
	}*/
	else {
		em.FlagComponents.GetComponent(id).set(ComponentFlags::NavigationWork);
		em.NavigationArchetype.NavigationComponents.GetComponent(id).target = target;
	}

	return false;
}
static bool TryAttackTarget(EntityId target, EntityId id, Vector2Int16 pos, EntityManager& em) {
	const auto& weapon = em.UnitArchetype.WeaponComponents.GetComponent(id);

	Vector2Int16 targetPos = em.PositionComponents.GetComponent(target);
	Rectangle16 collider = em.CollisionArchetype.ColliderComponents.GetComponent(target).collider;
	collider.position += targetPos;


	Vector2Int16 distance = collider.Closest(pos);
	distance -= pos;


	int range = ((int)(weapon.maxRange + 1) << 5);


	if (distance.LengthSquaredInt() < range * range) {

		if (weapon.IsReady())
			UnitEntityUtil::AttackTarget(id, target);

		return true;
	}

	return false;
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

		EntityId id = data.entities[i];
		const auto& pos = data.position[i];
		const auto& owner = data.owner[i];

		EntityId enemy = DetectNearbyEnemy(id, pos, owner, em);
		if (enemy != Entity::None) {

			UnitEntityUtil::SetAIState(id, UnitAIState::AttackTarget, enemy);
		}
	}
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



		if (target == id || !em.UnitArchetype.Archetype.HasEntity(target))
		{
			UnitEntityUtil::SetAIState(id, UnitAIState::Idle);
			continue;
		}

		if (!TryAttackTarget(target, id, pos, em)) {
			Vector2Int16 targetPos = em.PositionComponents.GetComponent(target);
			UnitEntityUtil::GoTo(id, targetPos);
		}
	}
}

void UnitAIGoToState::Think(UnitAIThinkData& data, EntityManager& em)
{
	int start = 0, end = data.size();
	for (int i = start; i < end; ++i) {
		EntityId id = data.entities[i];
		const auto& stateData = data.stateData[i];
		const auto& pos = data.position[i];

		if (TryMoveToPosition(id, pos, stateData.target.position, em)) {
			UnitEntityUtil::SetAIState(id, UnitAIState::Idle);
		}
	}
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
			if (!TryAttackTarget(target, id, pos, em)) {
				Vector2Int16 targetPos = em.PositionComponents.GetComponent(target);
				TryMoveToPosition(id, pos, targetPos, em);
			}
		}
		else
		{
			if (TryMoveToPosition(id, pos, stateData.target.position, em)) {
				UnitEntityUtil::SetAIState(id, UnitAIState::Idle);
			}
		}
	}
}

void UnitAIHoldPositionState::EnterState(UnitAIEnterStateData& data, EntityManager& em)
{
	for (EntityId id : data.entities) {
		em.UnitArchetype.StateComponents.GetComponent(id) = UnitState::Idle;
		em.FlagComponents.GetComponent(id).set(ComponentFlags::UnitStateChanged);
	}
}
void UnitAIHoldPositionState::Think(UnitAIThinkData& data, EntityManager& em)
{
	int start = 0, end = data.size();
	for (int i = start; i < end; ++i) {

		EntityId id = data.entities[i];
		const auto& pos = data.position[i];
		const auto& owner = data.owner[i];

		EntityId target = DetectNearbyEnemy(id, pos, owner, em);
		if (target != Entity::None) {
			TryAttackTarget(target, id, pos, em);
		}
	}
}

void UnitAIPatrolState::EnterState(UnitAIEnterStateData& data, EntityManager& em)
{
	for (EntityId id : data.entities) {
		auto& stateData = em.UnitArchetype.AIStateDataComponents.GetComponent(id);
		stateData.target2.position = em.PositionComponents.GetComponent(id);
	}
}
void UnitAIPatrolState::Think(UnitAIThinkData& data, EntityManager& em)
{
	int start = 0, end = data.size();
	for (int i = start; i < end; ++i) {
		EntityId id = data.entities[i];
		const auto& stateData = data.stateData[i];
		const auto& pos = data.position[i];

		EntityId target = DetectNearbyEnemy(id, pos, data.owner[i], em);

		if (target != Entity::None) {

			if (!TryAttackTarget(target, id, pos, em)) {
				Vector2Int16 targetPos = em.PositionComponents.GetComponent(target);
				TryMoveToPosition(id, pos, targetPos, em);
			}
		}
		else {

			if (TryMoveToPosition(id, pos, stateData.target.position, em)) {
				Vector2Int16 tmp = stateData.target.position;
				auto& st = em.UnitArchetype.AIStateDataComponents.GetComponent(id);
				st.target.position = stateData.target2.position;
				st.target2.position = tmp;
			}
		}
	}
}

void UnitAIFollowState::Think(UnitAIThinkData& data, EntityManager& em)
{
	int start = 0, end = data.size();
	for (int i = start; i < end; ++i) {

		EntityId id = data.entities[i];
		const auto& pos = data.position[i];
		const auto& owner = data.owner[i];
		const auto& stateData = data.stateData[i];

		EntityId enemy = DetectNearbyEnemy(id, pos, owner, em);

		if (enemy != Entity::None) {
			if (TryAttackTarget(enemy, id, pos, em))
				continue;
		}

		if (!em.UnitArchetype.Archetype.HasEntity(stateData.target.entityId))
		{
			UnitEntityUtil::SetAIState(id, UnitAIState::Idle);
			continue;
		}

		Vector2Int16 target = em.PositionComponents.GetComponent(stateData.target.entityId);
		Vector2Int16 distance = target - pos;
		if (distance.LengthSquaredInt() < 128 * 128)
		{
			auto& state = em.UnitArchetype.StateComponents.GetComponent(id);
			if (state != UnitState::Idle) {
				em.UnitArchetype.StateComponents.GetComponent(id) = UnitState::Idle;
				em.FlagComponents.GetComponent(id).set(ComponentFlags::UnitStateChanged);
			}
			continue;
		}

		TryMoveToPosition(id, pos, target, em);
	}
}

void UnitAIGatherResoureState::Think(UnitAIThinkData& data, EntityManager& em)
{
	int start = 0, end = data.size();
	for (int i = start; i < end; ++i) {
		EntityId id = data.entities[i];
		const auto& pos = data.position[i];
		const auto& owner = data.owner[i];
		const auto& stateData = data.stateData[i];


		Rectangle16 collider = em.CollisionArchetype.ColliderComponents.GetComponent(stateData.target.entityId).collider;

		collider.position += em.PositionComponents.GetComponent(stateData.target.entityId);

		Vector2Int16 target = collider.Closest(pos);

		if ((target - pos).LengthSquaredInt() > 250) {
			TryMoveToPosition(id, pos, target, em);
		}
		else {
			auto& state = em.UnitArchetype.StateComponents.GetComponent(id);

			if (state != UnitState::Mining)
			{
				em.UnitArchetype.StateComponents.GetComponent(id) = UnitState::Mining;
				em.UnitArchetype.StateDataComponents.GetComponent(id).target.entityId = stateData.target.entityId;
				em.FlagComponents.GetComponent(id).set(ComponentFlags::UnitStateChanged);
			}
			else {
				if (!em.FlagComponents.GetComponent(id).test(ComponentFlags::UpdateTimers)) {
					UnitEntityUtil::SetAIState(id, UnitAIState::Idle);
				}
			}
		}
	}
}
