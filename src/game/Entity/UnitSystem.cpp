#include "UnitSystem.h"
#include "../Profiler.h"
#include "EntityManager.h"
#include "EntityUtil.h"
#include "../Game.h"

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

		flags.clear(ComponentFlags::UpdateTimers);

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
			em.UnitArchetype.AnimationArchetype.OrientationArchetype.AnimOrientationComponents
				.GetComponent(id).CopyArray(unit.def->Graphics->IdleAnimations);

			flags.set(ComponentFlags::AnimationEnabled);
			flags.set(ComponentFlags::AnimationSetChanged);

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

			em.MovementArchetype.MovementComponents.GetComponent(id).velocity = { 0,0 };

			em.UnitArchetype.AnimationArchetype.OrientationArchetype.AnimOrientationComponents
				.GetComponent(id).CopyArray(unit.def->Graphics->AttackAnimations);

			flags.set(ComponentFlags::AnimationSetChanged);
			flags.set(ComponentFlags::AnimationEnabled);
			flags.clear(ComponentFlags::NavigationWork);

			if (unit.HasMovementGlow()) {
				EntityId glow = unit.movementGlowEntity;
				em.FlagComponents.GetComponent(glow).clear(ComponentFlags::AnimationEnabled);
				em.FlagComponents.GetComponent(glow).clear(ComponentFlags::RenderEnabled);
			}

			if (unit.def->Weapon->Sound.TotalClips > 0) {
				int i = std::rand() % unit.def->Weapon->Sound.TotalClips;
				Game::Audio.PlayClip(unit.def->Weapon->Sound.Clips[i], 1);

			}

			EntityUtil::StartTimer(id, unit.def->Weapon->Cooldown, TimerExpiredAction::WeaponAttack);

			break;
		}
		case UnitState::Death: {
			em.UnitArchetype.Archetype.RemoveEntity(id);
			em.CollisionArchetype.Archetype.RemoveEntity(id);
			em.NavigationArchetype.Archetype.RemoveEntity(id);
			em.MovementArchetype.Archetype.RemoveEntity(id);


			if (unit.HasMovementGlow()) {
				em.DeleteEntity(unit.movementGlowEntity);
				em.ParentArchetype.Archetype.RemoveEntity(id);
			}

			if (unit.def->Graphics->HasDeathAnimation()) {
				EntityUtil::PlayAnimation(id, unit.def->Graphics->DeathAnimation);
				EntityUtil::StartTimer(id, unit.def->Graphics->DeathAnimation.GetDuration(), TimerExpiredAction::UnitDeathAfterEffect);
			}
			else {
				EntityUtil::StartTimer(id, 1, TimerExpiredAction::UnitDeathAfterEffect);
			}

			int i = std::rand() % unit.def->Sounds.Death.TotalClips;
			Game::Audio.PlayClip(unit.def->Sounds.Death.Clips[i], 1);

			break;
		}
		}
	}
}
