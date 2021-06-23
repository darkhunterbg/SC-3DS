#include "UnitTurningState.h"
#include "../EntityManager.h"
#include "../EntityUtil.h"


void UnitTurningState::EnterState(
	UnitStateMachineChangeData& data, EntityManager& em)
{
	for (EntityId id : data.entities) {

		const UnitComponent& unit = em.UnitArchetype.UnitComponents.GetComponent(id);

		if (unit.def->Art.GetSprite().GetAnimation(AnimationType::Init))
			EntityUtil::PlayAnimation(id, *unit.def->Art.GetSprite().GetAnimation(AnimationType::Init));

		FlagsComponent& flags = em.FlagComponents.GetComponent(id);
	}
}
void UnitTurningState::ExitState(
	UnitStateMachineChangeData& data, EntityManager& em)
{
	for (EntityId id : data.entities) {
		FlagsComponent& flags = em.FlagComponents.GetComponent(id);
		const UnitComponent& unit = em.UnitArchetype.UnitComponents.GetComponent(id);
		flags.clear(ComponentFlags::NavigationWork);
	}
}