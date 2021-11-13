#include "UnitAIStateMachine.h"
#include "EntityManager.h"
#include "EntityUtil.h"

static constexpr const uint8_t Nav_None = 0xFF;

static constexpr const  uint16_t FollowMaxDistance = 96;

static const Vector2 movementTable8[]{
		{0,-1}, {0.8,-0.8},{1,0}, {0.8,0.8},{0,1}, {-0.8,0.8}, {-1,0},{-0.8,-0.8}
};
static std::vector<EntityId> scratch;
static uint8_t GetNavOrientation(EntityId id, Vector2Int16 dst, EntityManager& em)
{
	short velocity = 4;

	Vector2Int16 pos = em.GetPosition(id);

	if ((dst - pos).LengthSquaredInt() < velocity * velocity)
		return Nav_None;

	scratch.clear();

	int orien = EntityUtil::GetOrientationToPosition(id, dst);
	orien = (orien >> 2);// << 2;

	Rectangle16 collider = em.KinematicSystem.GetCollider(id).collider;

	for (int i = 0; i < 8; ++i)
	{
		orien = (orien + 1) % 8;
		Vector2Int16 move = Vector2Int16(movementTable8[orien] * velocity);
		move += pos;
		scratch.clear();

		Rectangle16 r = collider;
		r.position += move;

		em.KinematicSystem.RectCast(r, scratch);
		if (scratch.size() == 0 || (scratch.size() == 1 && scratch[0] == id))
		{
			return orien << 2;
		}
	}

	return Nav_None;

}
static EntityId DetectNearbyEnemy(EntityId id, Vector2Int16 pos, PlayerId owner, EntityManager& em)
{

	scratch.clear();

	UnitAIComponent ai = em.UnitSystem.GetAIComponent(id);

	short range = (int)(ai.seekRange + 1) << 5;
	Circle16 accquisitonRage = { pos , range };

	em.KinematicSystem.CircleCast(accquisitonRage, scratch);

	EntityId result = Entity::None;

	if (scratch.size() > 0)
	{
		int nearest = 0xFFFFFFFF / 2;

		for (EntityId e : scratch)
		{
			if (e == id)
				continue;

			if (!em.UnitSystem.IsUnit(e)) continue;

			PlayerId enemyOwner = em.UnitSystem.GetComponent(e).owner;

			if (enemyOwner != owner)
			{

				int t = (em.GetPosition(e) - pos).LengthSquaredInt();
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
static bool OnPosition(EntityId id, Vector2Int16 pos, Vector2Int16 target)
{
	return  (target - pos).LengthSquaredInt() < 25;
}
static bool IsTargetInAttackRange(EntityId id, EntityId target, Vector2Int16 pos, EntityManager& em)
{
	int dst = (em.GetPosition(target) - pos).LengthSquaredInt();
	auto& attack = em.UnitSystem.GetComponent(id).def->GetAttacks()[0];
	int atk = attack.MaxRange + 1;
	atk = atk << 5;

	bool inRange = dst <= atk * atk;
	return inRange;
}
static bool MoveToPosition(EntityId id, Vector2Int16 pos, EntityManager& em)
{
	int orien = GetNavOrientation(id, pos, em);
	if (orien == Nav_None)
	{
		EntityUtil::SetUnitState(id, UnitStateId::Idle);
		return false;
	};

	int current = em.GetOrientation(id);

	int turn = orien - current;
	/*	if (std::abs(turn) > def.Movement.RotationSpeed)
	{
	turn = def.Movement.RotationSpeed * sign(turn);
	}


	if (std::abs( (current - turn) - orien < std::abs( ((current + turn) %32) - orien)))
	{
	turn = -turn;
	}*/



	EntityUtil::SetOrientation(id, current + turn);

	if (em.UnitSystem.GetStateComponent(id).stateId != UnitStateId::Movement)
		EntityUtil::SetUnitState(id, UnitStateId::Movement);

	return true;
}
static bool Attack(EntityId id, EntityId enemy, EntityManager& em)
{
	if (em.UnitSystem.GetAIComponent(id).IsAttackReady())
	{
		uint8_t orientation = EntityUtil::GetOrientationToPosition(id, em.GetPosition(enemy));
		em.SetOrientation(id, orientation);
		em.UnitSystem.GetAIComponent(id).targetEntity = enemy;

		EntityUtil::SetUnitState(id, UnitStateId::Attack);
		return true;
	}

	return false;
}

void UnitAIState::Start(int batch)
{
	thinkData.start = 0;
	thinkData.end = std::min((int)thinkData.size(), batch);
}
void UnitAIState::Advance(int batch)
{
	thinkData.start = thinkData.end;
	thinkData.end = std::min((int)thinkData.size(), thinkData.end + batch);
}

void UnitAIStateMachine::CreateStates(std::vector< UnitAIState*>& states)
{
	states.clear();
	states.resize(UnitAIStateTypeCount);

	states[(int)UnitAIStateId::Idle] = new UnitAIState(IdleAggresiveThink);
	states[(int)UnitAIStateId::AttackTarget] = new UnitAIState(AttackTargetThink);
	states[(int)UnitAIStateId::GoTo] = new UnitAIState(GoToThink);
	states[(int)UnitAIStateId::GoToAttack] = new UnitAIState(GoToAttackThink);
	states[(int)UnitAIStateId::Patrol] = new UnitAIState(PatrolThink);
	states[(int)UnitAIStateId::HoldPosition] = new UnitAIState(HoldPositionThink);
	states[(int)UnitAIStateId::Follow] = new UnitAIState(FollowThink);
}


void UnitAIStateMachine::IdleAggresiveThink(UnitAIThinkData& data, EntityManager& em)
{
	for (int i = data.start; i < data.end; ++i)
	{
		EntityId id = data.entities[i];
		Vector2Int16 pos = em.GetPosition(id);
		PlayerId owner = em.UnitSystem.GetComponent(id).owner;

		EntityId enemy = em.UnitSystem.GetComponent(id).attackedBy;

		if (enemy == Entity::None)
			enemy = DetectNearbyEnemy(id, pos, owner, em);


		if (enemy != Entity::None)
		{
			em.UnitSystem.GetAIComponent(id).targetPosition = em.GetPosition(enemy);
			EntityUtil::SetUnitAIState(id, UnitAIStateId::GoToAttack);
		}
		else if (em.UnitSystem.GetStateComponent(id).stateId != UnitStateId::Idle)
			EntityUtil::SetUnitState(id, UnitStateId::Idle);
	}
}


void UnitAIStateMachine::AttackTargetThink(UnitAIThinkData& data, EntityManager& em)
{
	for (int i = data.start; i < data.end; ++i)
	{
		EntityId id = data.entities[i];
		Vector2Int16 pos = em.GetPosition(id);
		PlayerId owner = em.UnitSystem.GetComponent(id).owner;

		EntityId enemy = em.UnitSystem.GetAIComponent(id).targetEntity;

		if (enemy!= Entity::None && em.HasEntity(enemy) && em.UnitSystem.IsUnit(enemy))
		{
			const UnitComponent& unit = em.UnitSystem.GetComponent(id);
			const UnitDef& def = *unit.def;

			if (!IsTargetInAttackRange(id, enemy, pos, em))
			{
				MoveToPosition(id, em.GetPosition(enemy), em);
				continue;
			}


			Attack(id, enemy, em);

		}
		else
		{
			EntityUtil::SetUnitAIState(id, UnitAIStateId::Idle);
		}
	}
}

void UnitAIStateMachine::GoToThink(UnitAIThinkData& data, EntityManager& em)
{
	for (int i = data.start; i < data.end; ++i)
	{
		EntityId id = data.entities[i];
		const UnitComponent& unit = em.UnitSystem.GetComponent(id);
		UnitAIComponent& ai = em.UnitSystem.GetAIComponent(id);
		const UnitDef& def = *unit.def;
		Vector2Int16 pos = em.GetPosition(id);

		Vector2Int16 dst = ai.targetPosition;
		if (OnPosition(id, pos, dst))
		{
			EntityUtil::SetUnitAIState(id, UnitAIStateId::Idle);
		}
		else
		{
			MoveToPosition(id, dst, em);
		}
	}

}

void UnitAIStateMachine::GoToAttackThink(UnitAIThinkData& data, EntityManager& em)
{
	for (int i = data.start; i < data.end; ++i)
	{
		EntityId id = data.entities[i];
		const UnitComponent& unit = em.UnitSystem.GetComponent(id);
		UnitAIComponent& ai = em.UnitSystem.GetAIComponent(id);
		const UnitDef& def = *unit.def;
		Vector2Int16 pos = em.GetPosition(id);

		Vector2Int16 dst = ai.targetPosition;
		if (OnPosition(id, pos, dst))
		{
			EntityUtil::SetUnitAIState(id, UnitAIStateId::Idle);
		}
		else
		{
			EntityId enemy = DetectNearbyEnemy(id, pos, unit.owner, em);

			if (enemy != Entity::None)
			{

				if (IsTargetInAttackRange(id, enemy, pos, em))
				{
					Attack(id, enemy, em);
					continue;
				}

			}

			MoveToPosition(id, dst, em);
		}
	}
}

void UnitAIStateMachine::PatrolThink(UnitAIThinkData& data, EntityManager& em)
{
	for (int i = data.start; i < data.end; ++i)
	{
		EntityId id = data.entities[i];
		const UnitComponent& unit = em.UnitSystem.GetComponent(id);
		UnitAIComponent& ai = em.UnitSystem.GetAIComponent(id);
		const UnitDef& def = *unit.def;
		Vector2Int16 pos = em.GetPosition(id);

		Vector2Int16 dst = ai.targetPosition;
		if (OnPosition(id, pos, dst))
		{
			ai.targetPosition = ai.targetPosition2;
			ai.targetPosition2 = dst;
		}
		else
		{
			EntityId enemy = DetectNearbyEnemy(id, pos, unit.owner, em);

			if (enemy != Entity::None)
			{
				if (IsTargetInAttackRange(id, enemy, pos, em))
				{
					Attack(id, enemy, em);
					continue;
				}

			}

			MoveToPosition(id, dst, em);
		}
	}
}

void UnitAIStateMachine::HoldPositionThink(UnitAIThinkData& data, EntityManager& em)
{
	for (int i = data.start; i < data.end; ++i)
	{
		EntityId id = data.entities[i];
		Vector2Int16 pos = em.GetPosition(id);
		PlayerId owner = em.UnitSystem.GetComponent(id).owner;

		EntityId enemy = DetectNearbyEnemy(id, pos, owner, em);
		if (enemy != Entity::None)
		{
			if (IsTargetInAttackRange(id, enemy, pos, em))
			{
				Attack(id, enemy, em);
				continue;
			}
		}
		if (em.UnitSystem.GetStateComponent(id).stateId != UnitStateId::Idle)
			EntityUtil::SetUnitState(id, UnitStateId::Idle);
	}
}

void UnitAIStateMachine::FollowThink(UnitAIThinkData& data, EntityManager& em)
{
	for (int i = data.start; i < data.end; ++i)
	{
		EntityId id = data.entities[i];
		const UnitComponent& unit = em.UnitSystem.GetComponent(id);
		UnitAIComponent& ai = em.UnitSystem.GetAIComponent(id);
		const UnitDef& def = *unit.def;
		Vector2Int16 pos = em.GetPosition(id);

		Vector2Int16 dst = em.GetPosition(ai.targetEntity);
		if ((pos - dst).LengthSquaredInt() > FollowMaxDistance * FollowMaxDistance)
		{
			MoveToPosition(id, dst, em);
		}
		else
		{
			if (em.UnitSystem.GetStateComponent(id).stateId != UnitStateId::Idle)
				EntityUtil::SetUnitState(id, UnitStateId::Idle);
		}
	}
}

