#include "UnitAIStateMachine.h"
#include "EntityManager.h"
#include "EntityUtil.h"


static std::vector<EntityId> scratch;
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

				int t = (em.GetPosition(id) - pos).LengthSquaredInt();
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


void UnitAIState::StartEnterState(int batch)
{
	enterStateData.start = 0;
	enterStateData.end = std::min((int)enterStateData.size(), batch);
}
void UnitAIState::AdvanceEnterState(int batch)
{
	enterStateData.start = enterStateData.end;
	enterStateData.end = std::min((int)enterStateData.size(), enterStateData.end + batch);
}
void UnitAIState::StartThink(int batch)
{
	thinkData.start = 0;
	thinkData.end = std::min((int)thinkData.size(), batch);
}
void UnitAIState::AdvanceThink(int batch)
{
	thinkData.start = thinkData.end;
	thinkData.end = std::min((int)thinkData.size(), thinkData.end + batch);
}


void UnitAIStateMachine::CreateStates(std::vector< UnitAIState*>& states)
{
	states.clear();
	states.resize(UnitAIStateTypeCount);

	states[(int)UnitAIStateId::IdleAggressive] = new UnitAIState(IdleEnter, IdleAggresiveThink);
	states[(int)UnitAIStateId::IdlePassive] = new UnitAIState(IdleEnter, nullptr);
	states[(int)UnitAIStateId::AttackTarget] = new UnitAIState(nullptr, AttackTargetThink);
	states[(int)UnitAIStateId::AttackLoop] = new UnitAIState(AttackLoopEnter, AttackLoopThink);
	states[(int)UnitAIStateId::AttackExit] = new UnitAIState(EndAttackEnter, EndAttackThink);
	states[(int)UnitAIStateId::Walk] = new UnitAIState(WalkEnter, IdleAggresiveThink);
}


void UnitAIStateMachine::IdleEnter(UnitAIEnterStateData& data, EntityManager& em)
{
	for (int i = data.start; i < data.end; ++i)
	{
		EntityId id = data.entities[i];
		const UnitComponent& unit = em.UnitSystem.GetComponent(id);
		const UnitDef& def = *unit.def;
		EntityUtil::PlayAnimation(id, *def.Art.GetSprite().GetAnimation(AnimationType::Init), def.Art.GetShadowImage());
	}
}

void UnitAIStateMachine::IdleAggresiveThink(UnitAIThinkData& data, EntityManager& em)
{
	for (int i = data.start; i < data.end; ++i)
	{
		EntityId id = data.entities[i];
		Vector2Int16 pos = em.GetPosition(id);
		PlayerId owner = em.UnitSystem.GetComponent(id).owner;

		EntityId enemy = DetectNearbyEnemy(id, pos, owner, em);
		if (enemy != Entity::None)
		{
			EntityUtil::SetUnitAIState(id, UnitAIStateId::AttackTarget);
		}
	}
}



void UnitAIStateMachine::AttackTargetThink(UnitAIThinkData& data, EntityManager& em)
{
	for (int i = data.start; i < data.end; ++i)
	{
		EntityId id = data.entities[i];
		Vector2Int16 pos = em.GetPosition(id);
		PlayerId owner = em.UnitSystem.GetComponent(id).owner;

		EntityId enemy = DetectNearbyEnemy(id, pos, owner, em);


		if (enemy != Entity::None)
		{
			const UnitComponent& unit = em.UnitSystem.GetComponent(id);
			const UnitDef& def = *unit.def;

			uint8_t orientation = EntityUtil::GetOrientationToPosition(id, em.GetPosition(enemy));

			em.SetOrientation(id, orientation);

			auto anim = def.Art.GetSprite().GetAnimation(AnimationType::GroundAttackInit);

			if (em.AnimationSystem.GetComponent(id).clip != anim)
				EntityUtil::PlayAnimation(id, *anim, def.Art.GetShadowImage());
			else
			{
				if (em.AnimationSystem.GetComponent(id).done)
				{
					em.UnitSystem.GetAIComponent(id).targetEntity = enemy;
					EntityUtil::SetUnitAIState(id, UnitAIStateId::AttackLoop);
				}
			}
		}
		else
		{
			EntityUtil::SetUnitAIState(id, em.UnitSystem.GetAIComponent(id).idleStateId);
			// TODO: AI system support wait for animation flag, blocking updates of AI until animation is completed
			continue;
		}

	}
}

void UnitAIStateMachine::AttackLoopEnter(UnitAIEnterStateData& data, EntityManager& em)
{
	for (int i = data.start; i < data.end; ++i)
	{
		EntityId id = data.entities[i];
		const UnitComponent& unit = em.UnitSystem.GetComponent(id);
		const UnitDef& def = *unit.def;
		EntityUtil::PlayAnimation(id, *def.Art.GetSprite().GetAnimation(AnimationType::GroundAttackRepeat), def.Art.GetShadowImage());
	}
}

void UnitAIStateMachine::AttackLoopThink(UnitAIThinkData& data, EntityManager& em)
{
	for (int i = data.start; i < data.end; ++i)
	{
		EntityId id = data.entities[i];
		const UnitComponent& unit = em.UnitSystem.GetComponent(id);
		const UnitDef& def = *unit.def;

		if (!em.AnimationSystem.GetComponent(id).done)
			continue;

		Vector2Int16 pos = em.GetPosition(id);
		PlayerId owner = em.UnitSystem.GetComponent(id).owner;
		EntityId enemy = DetectNearbyEnemy(id, pos, owner, em);
		if (enemy != Entity::None)
		{
			uint8_t orientation = EntityUtil::GetOrientationToPosition(id, em.GetPosition(enemy));

			em.SetOrientation(id, orientation);

			if (!em.UnitSystem.GetAIComponent(id).IsAttackReady()) continue;

			em.UnitSystem.GetAIComponent(id).targetEntity = enemy;
			em.UnitSystem.GetAIComponent(id).attackId = 0;
			EntityUtil::SetUnitAIState(id, UnitAIStateId::AttackLoop);
			continue;
		}
		else
		{
			EntityUtil::SetUnitAIState(id, UnitAIStateId::AttackExit);
			// TODO: AI system support wait for animation flag, blocking updates of AI until animation is completed
			continue;
		}
	}
}

void UnitAIStateMachine::EndAttackEnter(UnitAIEnterStateData& data, EntityManager& em)
{
	for (int i = data.start; i < data.end; ++i)
	{
		EntityId id = data.entities[i];
		const UnitComponent& unit = em.UnitSystem.GetComponent(id);
		const UnitDef& def = *unit.def;
		EntityUtil::PlayAnimation(id, *def.Art.GetSprite().GetAnimation(AnimationType::GroundAttackToIdle), def.Art.GetShadowImage());
	}
}

void UnitAIStateMachine::EndAttackThink(UnitAIThinkData& data, EntityManager& em)
{
	for (int i = data.start; i < data.end; ++i)
	{
		EntityId id = data.entities[i];
		const UnitComponent& unit = em.UnitSystem.GetComponent(id);
		const UnitDef& def = *unit.def;


		Vector2Int16 pos = em.GetPosition(id);
		PlayerId owner = em.UnitSystem.GetComponent(id).owner;
		EntityId enemy = DetectNearbyEnemy(id, pos, owner, em);

		if (enemy != Entity::None)
		{
			em.UnitSystem.GetAIComponent(id).targetEntity = enemy;
			EntityUtil::SetUnitAIState(id, UnitAIStateId::AttackTarget);
		}
		else
		{
			if (em.AnimationSystem.GetComponent(id).done)
				EntityUtil::SetUnitAIState(id, em.UnitSystem.GetAIComponent(id).idleStateId);
		}

	}
}

void UnitAIStateMachine::WalkEnter(UnitAIEnterStateData& data, EntityManager& em)
{
	for (int i = data.start; i < data.end; ++i)
	{
		EntityId id = data.entities[i];
		const UnitComponent& unit = em.UnitSystem.GetComponent(id);
		const UnitDef& def = *unit.def;
		EntityUtil::PlayAnimation(id, *def.Art.GetSprite().GetAnimation(AnimationType::Walking), def.Art.GetShadowImage());
	}
}
