#include "UnitSystem.h"
#include "../Profiler.h"
#include "EntityManager.h"
#include "EntityUtil.h"

void UnitSystem::ApplyUnitState(EntityManager& em) {
	stateChanged.clear();

	for (EntityId id : em.UnitArchetype.Archetype.GetEntities()) {

		FlagsComponent& flags = em.FlagComponents.GetComponent(id);
		if (flags.test(ComponentFlags::UnitStateChanged)) {
			flags.clear(ComponentFlags::UnitStateChanged);

			stateChanged.push_back(id);
			continue;
		}
	}


	int start = 0;
	int end = stateChanged.size();

	for (int i = start; i < end; ++i) {
		EntityId id = stateChanged[i];

		UnitState state = em.UnitArchetype.StateComponents.GetComponent(id);
		const UnitComponent& unit = em.UnitArchetype.UnitComponents.GetComponent(id);

		FlagsComponent& flags = em.FlagComponents.GetComponent(id);

		switch (state) {
		case UnitState::Idle: {

			em.UnitArchetype.AnimationArchetype.OrientationArchetype.AnimOrientationComponents
				.GetComponent(id).CopyArray(unit.def->Graphics->IdleAnimations);

			em.MovementArchetype.MovementComponents.GetComponent(id).velocity = { 0,0 };

			flags.clear(ComponentFlags::NavigationWork);
			flags.set(ComponentFlags::AnimationEnabled);
			flags.set(ComponentFlags::AnimationSetChanged);

			if (unit.HasMovementGlow()) {
				EntityId glow = unit.movementGlowEntity;
				em.FlagComponents.GetComponent(glow).clear(ComponentFlags::AnimationEnabled);
				em.FlagComponents.GetComponent(glow).clear(ComponentFlags::RenderEnabled);
			}

			break;
		}
		case UnitState::Turning: {

			em.MovementArchetype.MovementComponents.GetComponent(id).velocity = { 0,0 };

			flags.set(ComponentFlags::AnimationEnabled);

			if (unit.HasMovementGlow()) {
				EntityId glow = unit.movementGlowEntity;

				em.FlagComponents.GetComponent(glow).set(ComponentFlags::AnimationEnabled);
				em.FlagComponents.GetComponent(glow).set(ComponentFlags::RenderEnabled);
			}

			break;
		}
		case UnitState::Movement: {

			em.UnitArchetype.AnimationArchetype.OrientationArchetype.AnimOrientationComponents
				.GetComponent(id).CopyArray(unit.def->Graphics->MovementAnimations);

			flags.set(ComponentFlags::AnimationSetChanged);
			flags.set(ComponentFlags::AnimationEnabled);

			if (unit.HasMovementGlow()) {
				EntityId glow = unit.movementGlowEntity;
				em.FlagComponents.GetComponent(glow).set(ComponentFlags::AnimationEnabled);
				em.FlagComponents.GetComponent(glow).set(ComponentFlags::AnimationFrameChanged);
				em.FlagComponents.GetComponent(glow).set(ComponentFlags::RenderEnabled);
				em.FlagComponents.GetComponent(glow).set(ComponentFlags::OrientationChanged);
			}

			break;
		}
		case UnitState::Attacking: {

			em.UnitArchetype.AnimationArchetype.OrientationArchetype.AnimOrientationComponents
				.GetComponent(id).CopyArray(unit.def->Graphics->AttackAnimations);

			flags.set(ComponentFlags::AnimationSetChanged);
			flags.set(ComponentFlags::AnimationEnabled);

			if (unit.HasMovementGlow()) {
				EntityId glow = unit.movementGlowEntity;
				em.FlagComponents.GetComponent(glow).clear(ComponentFlags::AnimationEnabled);
				em.FlagComponents.GetComponent(glow).clear(ComponentFlags::RenderEnabled);
			}

			break;
		}
		}
	}
}
