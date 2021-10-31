#include "UnitAIStateMachine.h"
#include "EntityManager.h"
#include "EntityUtil.h"

std::vector<IUnitAIState*>  IUnitAIState::States = {
	new UnitAIStates::IdleAggressive(),
	new UnitAIStates::IdlePassive(),
	new UnitAIStates::AttackTarget(),
	new UnitAIStates::AttackLoop(),
};


enum class UnitAIState :uint8_t {

	IdleAggressive = 0,
	IdlePassive = 1,
	AttackTarget = 2,
	AttackLoop = 3,
};

void IUnitAIState::EnterState(UnitAIEnterStateData& data, EntityManager& em)
{
}

static void SetUnitAIState(EntityId id, UnitAIState state)
{
	auto& ai = EntityUtil::GetManager().UnitSystem.GetAIComponent(id);
	ai.stateId = (uint8_t)state;
	ai.newState = true;
}

namespace UnitAIStates
{
	static std::vector<EntityId> scratch;
	static EntityId DetectNearbyEnemy(EntityId id, Vector2Int16 pos, PlayerId owner, EntityManager& em)
	{

		scratch.clear();

		const auto& weapon = em.UnitSystem.GetComponent(id).def->GetAttacks()[0];

		short range = (int)(weapon.Range.y + 1) << 5;
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

	void IdleAggressive::EnterState(UnitAIEnterStateData& data, EntityManager& em)
	{
		for (EntityId id : data.entities)
		{
			const UnitComponent& unit = em.UnitSystem.GetComponent(id);
			const UnitDef& def = *unit.def;
			EntityUtil::PlayAnimation(id, *def.Art.GetSprite().GetAnimation(AnimationType::Init), def.Art.GetShadowImage());
		}
	}
	void IdleAggressive::Think(UnitAIThinkData& data, EntityManager& em)
	{
		int start = 0, end = data.size();
		for (int i = start; i < end; ++i)
		{
			EntityId id = data.entities[i];
			Vector2Int16 pos = em.GetPosition(id);
			PlayerId owner = em.UnitSystem.GetComponent(id).owner;

			EntityId enemy = DetectNearbyEnemy(id, pos, owner, em);
			if (enemy != Entity::None)
			{
				SetUnitAIState(id, UnitAIState::AttackTarget);
			}
		}
	}

	void IdlePassive::EnterState(UnitAIEnterStateData& data, EntityManager& em)
	{
		for (EntityId id : data.entities)
		{
			const UnitComponent& unit = em.UnitSystem.GetComponent(id);
			const UnitDef& def = *unit.def;
			EntityUtil::PlayAnimation(id, *def.Art.GetSprite().GetAnimation(AnimationType::Init), def.Art.GetShadowImage());
		}
	}
	void IdlePassive::Think(UnitAIThinkData& data, EntityManager& em)
	{

	}

	void AttackTarget::Think(UnitAIThinkData& data, EntityManager& em)
	{
		int start = 0, end = data.size();
		for (int i = start; i < end; ++i)
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
						SetUnitAIState(id, UnitAIState::AttackLoop);
					}
				}
			}
		}

	}

	void AttackLoop::EnterState(UnitAIEnterStateData& data, EntityManager& em)
	{
		for (EntityId id : data.entities)
		{
			const UnitComponent& unit = em.UnitSystem.GetComponent(id);
			const UnitDef& def = *unit.def;
			EntityUtil::PlayAnimation(id, *def.Art.GetSprite().GetAnimation(AnimationType::GroundAttackRepeat), def.Art.GetShadowImage());
		}
	}
	void AttackLoop::Think(UnitAIThinkData& data, EntityManager& em)
	{
		int start = 0, end = data.size();
		for (int i = start; i < end; ++i)
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

				SetUnitAIState(id, UnitAIState::AttackLoop);
				continue;
			}
			else
			{
				SetUnitAIState(id, (UnitAIState)em.UnitSystem.GetAIComponent(id).idleStateId);
				// TODO: AI system support wait for animation flag, blocking updates of AI until animation is completed
				continue;
			}
		}

	}
}

