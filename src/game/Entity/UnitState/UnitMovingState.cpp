#include "UnitMovingState.h"
#include "../EntityManager.h"
#include "../EntityUtil.h"

static const Vector2 movementTable32[]{
	{0,-1}, {0,-1},
	{0.7,-0.7},{0.7,-0.7},{0.7,-0.7},{0.7,-0.7},
	{1,0}, {1,0}, {1,0}, {1,0},
	{0.7,0.7},{0.7,0.7},{0.7,0.7},{0.7,0.7},
	{0,1},{0,1},{0,1},{0,1},
	{-0.7,0.7},{-0.7,0.7},{-0.7,0.7},{-0.7,0.7},
	{-1,0}, {-1,0}, {-1,0}, {-1,0},
	{-0.7,-0.7},{-0.7,-0.7},{-0.7,-0.7},{-0.7,-0.7},
	{0,-1},{0,-1}

};


void UnitMovingState::EnterState(
	UnitStateMachineChangeData& data, EntityManager& em)
{
	for (EntityId id : data.entities) {

		const UnitComponent& unit = em.UnitArchetype.UnitComponents.GetComponent(id);
		FlagsComponent& flags = em.FlagComponents.GetComponent(id);
		auto& movement = em.MovementArchetype.MovementComponents.GetComponent(id);
		const auto& orientation = em.OrientationComponents.GetComponent(id);
		const auto& velocity = em.UnitArchetype.MovementComponents.GetComponent(id).movementSpeed;

		if (unit.def->Art.GetSprite().GetAnimation(AnimationType::Walking))
			EntityUtil::PlayAnimation(id, *unit.def->Art.GetSprite().GetAnimation(AnimationType::Walking));

		flags.set(ComponentFlags::NavigationWork);
		flags.set(ComponentFlags::AnimationEnabled);

		movement.velocity = Vector2Int8(movementTable32[orientation] * velocity);

	}
}

void UnitMovingState::ExitState(
	UnitStateMachineChangeData& data, EntityManager& em)
{
	for (EntityId id : data.entities) {
		FlagsComponent& flags = em.FlagComponents.GetComponent(id);
		const UnitComponent& unit = em.UnitArchetype.UnitComponents.GetComponent(id);
		em.MovementArchetype.MovementComponents.GetComponent(id).velocity = { 0,0 };
		flags.clear(ComponentFlags::NavigationWork);
	}
}

