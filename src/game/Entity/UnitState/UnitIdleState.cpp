#include "UnitIdleState.h"
#include "../EntityManager.h"
#include "../EntityUtil.h"

void UnitIdleState::EnterState(
	UnitStateMachineChangeData& data, EntityManager& em)
{
	for (EntityId id : data.entities) {

		const UnitComponent& unit = em.UnitArchetype.UnitComponents.GetComponent(id);

		if (unit.def->Art.GetSprite().GetAnimation(AnimationType::Init))
			EntityUtil::PlayAnimation(id, *unit.def->Art.GetSprite().GetAnimation(AnimationType::Init));

	}
}
void UnitIdleState::ExitState(
	UnitStateMachineChangeData& data, EntityManager& em)
{
	// Nothing for exiting idle state
}