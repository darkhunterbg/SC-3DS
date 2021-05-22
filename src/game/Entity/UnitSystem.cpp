#include "UnitSystem.h"
#include "../Profiler.h"
#include "EntityManager.h"
#include "EntityUtil.h"


void UnitSystem::ApplyUnitState(EntityManager& em) {

	scratch.clear();

	for (EntityId id : em.UnitArchetype.Archetype.GetEntities()) {
		FlagsComponent& flags = em.FlagComponents.GetComponent(id);
		if (flags.test(ComponentFlags::UnitStateChanged)) {
			flags.clear(ComponentFlags::UnitStateChanged);

			scratch.push_back(id);
		}
	}

	int start = 0;
	int end = scratch.size();

	for (int i = start; i < end; ++i) {
		EntityId id = scratch[i];

		UnitState state = em.UnitArchetype.StateComponents.GetComponent(id);
		uint8_t orientation = em.UnitArchetype.OrientationComponents.GetComponent(id);
		const UnitComponent& unit = em.UnitArchetype.UnitComponents.GetComponent(id);

		FlagsComponent& flags = em.FlagComponents.GetComponent(id);

		switch (state) {
		case UnitState::Idle: {

			em.MovementArchetype.MovementComponents.GetComponent(id).velocity = { 0,0 };

			const UnitAnimationClip& clip = unit.def->Graphics->IdleAnimations[orientation];

			em.UnitArchetype.AnimationArchetype.AnimationComponents.GetComponent(id).clip = &clip;
			em.UnitArchetype.AnimationArchetype.TrackerComponents.GetComponent(id).PlayClip(&clip);

			flags.clear(ComponentFlags::NavigationWork);
			flags.set(ComponentFlags::AnimationEnabled);
			flags.set(ComponentFlags::AnimationFrameChanged);

			if (unit.HasMovementGlow()) {
				EntityId glow = unit.movementGlowEntity;
				em.FlagComponents.GetComponent(glow).clear(ComponentFlags::AnimationEnabled);
				em.FlagComponents.GetComponent(glow).clear(ComponentFlags::RenderEnabled);
			}

			break;
		}
		case UnitState::Turning: {

			em.MovementArchetype.MovementComponents.GetComponent(id).velocity = { 0,0 };

			const UnitAnimationClip& clip = unit.def->Graphics->IdleAnimations[orientation];
			em.UnitArchetype.AnimationArchetype.AnimationComponents.GetComponent(id).clip = &clip;
			em.UnitArchetype.AnimationArchetype.TrackerComponents.GetComponent(id).PlayClip(&clip);

			flags.set(ComponentFlags::AnimationEnabled);
			flags.set(ComponentFlags::AnimationFrameChanged);

			if (unit.HasMovementGlow()) {
				EntityId glow = unit.movementGlowEntity;
				
				const AnimationClip& clip = unit.def->Graphics->MovementGlowAnimations[orientation];

				em.AnimationArchetype.AnimationComponents.GetComponent(glow).clip = &clip;
				em.AnimationArchetype.TrackerComponents.GetComponent(glow).PlayClip(&clip);


				em.FlagComponents.GetComponent(glow).set(ComponentFlags::AnimationEnabled);
				em.FlagComponents.GetComponent(glow).set(ComponentFlags::AnimationFrameChanged);
				em.FlagComponents.GetComponent(glow).set(ComponentFlags::RenderEnabled);
			}

			break;
		}
		case UnitState::Movement: {
			const UnitAnimationClip& clip = unit.def->Graphics->MovementAnimations[orientation];
			em.UnitArchetype.AnimationArchetype.AnimationComponents.GetComponent(id).clip = &clip;
			em.UnitArchetype.AnimationArchetype.TrackerComponents.GetComponent(id).PlayClip(&clip);

			flags.set(ComponentFlags::AnimationEnabled);
			flags.set(ComponentFlags::AnimationFrameChanged);

			if (unit.HasMovementGlow()) {
				EntityId glow = unit.movementGlowEntity;

				const AnimationClip& clip = unit.def->Graphics->MovementGlowAnimations[orientation];

				em.AnimationArchetype.AnimationComponents.GetComponent(glow).clip = &clip;
				em.AnimationArchetype.TrackerComponents.GetComponent(glow).PlayClip(&clip);

				em.FlagComponents.GetComponent(glow).set(ComponentFlags::AnimationEnabled);
				em.FlagComponents.GetComponent(glow).set(ComponentFlags::AnimationFrameChanged);
				em.FlagComponents.GetComponent(glow).set(ComponentFlags::RenderEnabled);
			}

			break;
		}
		}
	}
}
