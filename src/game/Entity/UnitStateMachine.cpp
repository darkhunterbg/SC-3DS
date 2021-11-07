#include "UnitStateMachine.h"
#include "EntityManager.h"
#include "EntityUtil.h"

#include "MathLib.h"

void UnitStateData::Start(int batch)
{
	start = 0;
	end = std::min((int)entities.size(), batch);
}
void UnitStateData::Advance(int batch)
{
	start = end;
	end = std::min((int)entities.size(), end + batch);
}

void UnitStateMachine::CreateStates(std::vector<UnitState*>& states)
{
	states.resize(UnitStateCount);
	states.shrink_to_fit();
	states[(int)UnitStateId::Idle] = new UnitState(IdleEnter, nullptr);
	states[(int)UnitStateId::Movement] = new UnitState(MovementEnter, nullptr);
	states[(int)UnitStateId::Attack] = new UnitState(AttackEnter, AttackExit);
}

void UnitStateMachine::IdleEnter(UnitStateData& data, EntityManager& em)
{
	for (int i = data.start; i < data.end; ++i)
	{
		EntityId id = data.entities[i];
		const UnitComponent& unit = em.UnitSystem.GetComponent(id);
		const UnitDef& def = *unit.def;
		EntityUtil::PlayAnimation(id, *def.Art.GetSprite().GetAnimation(AnimationType::Init), def.Art.GetShadowImage());
	}
}

void UnitStateMachine::MovementEnter(UnitStateData& data, EntityManager& em)
{
	for (int i = data.start; i < data.end; ++i)
	{
		EntityId id = data.entities[i];
		const UnitComponent& unit = em.UnitSystem.GetComponent(id);
		const UnitDef& def = *unit.def;
		EntityUtil::PlayAnimation(id, *def.Art.GetSprite().GetAnimation(AnimationType::Walking), def.Art.GetShadowImage());
	}
}

void UnitStateMachine::AttackEnter(UnitStateData& data, EntityManager& em)
{
	for (int i = data.start; i < data.end; ++i)
	{
		EntityId id = data.entities[i];
		const UnitComponent& unit = em.UnitSystem.GetComponent(id);
		const UnitDef& def = *unit.def;

		if (data.otherState[i] != UnitStateId::Attack)
		{
			EntityUtil::PlayAnimation(id, *def.Art.GetSprite().GetAnimation(AnimationType::GroundAttackInit), def.Art.GetShadowImage());
		}
		else
		{
			EntityUtil::PlayAnimation(id, *def.Art.GetSprite().GetAnimation(AnimationType::GroundAttackRepeat), def.Art.GetShadowImage());
		}
	}
}

void UnitStateMachine::AttackExit(UnitStateData& data, EntityManager& em)
{
	for (int i = data.start; i < data.end; ++i)
	{
		EntityId id = data.entities[i];
		const UnitComponent& unit = em.UnitSystem.GetComponent(id);
		const UnitDef& def = *unit.def;

		if (data.otherState[i] == UnitStateId::Idle)
			EntityUtil::PlayAnimation(id, *def.Art.GetSprite().GetAnimation(AnimationType::GroundAttackToIdle), def.Art.GetShadowImage());
	}
}

