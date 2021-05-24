#include "UnitStateMachine.h"
#include "EntityManager.h"
#include "EntityUtil.h"
#include "../Game.h"
#include "../Profiler.h"

#include <algorithm>

static std::vector<EntityId> scratch;


// ============================ Idle State ====================================

std::array<IUnitState*, UnitStatesCount> UnitStateMachine::States = {
	new UnitIdleState(),
	new UnitTurningState(), 
	new UnitMovingState(),
	new UnitAttackingState(), 
	new UnitDeathState()
};

void UnitIdleState::EnterState(
	UnitStateMachinChangeData& data, EntityManager& em)
{
	for (EntityId id : data.entities) {

		const UnitComponent& unit = em.UnitArchetype.UnitComponents.GetComponent(id);

		em.UnitArchetype.AnimationArchetype.OrientationArchetype.AnimOrientationComponents
			.GetComponent(id).CopyArray(unit.def->Graphics->IdleAnimations);

		FlagsComponent& flags = em.FlagComponents.GetComponent(id);

		flags.set(ComponentFlags::AnimationEnabled);
		flags.set(ComponentFlags::AnimationSetChanged);
	}
}
void UnitIdleState::ExitState(
	UnitStateMachinChangeData& data, EntityManager& em)
{
	// Nothing for exiting idle state
}


// ============================ Turning State ====================================

void UnitTurningState::EnterState(
	UnitStateMachinChangeData& data, EntityManager& em)
{
	for (EntityId id : data.entities) {

		const UnitComponent& unit = em.UnitArchetype.UnitComponents.GetComponent(id);

		em.UnitArchetype.AnimationArchetype.OrientationArchetype.AnimOrientationComponents
			.GetComponent(id).CopyArray(unit.def->Graphics->IdleAnimations);

		FlagsComponent& flags = em.FlagComponents.GetComponent(id);

		flags.set(ComponentFlags::NavigationWork);
		flags.set(ComponentFlags::AnimationEnabled);
		flags.set(ComponentFlags::AnimationSetChanged);

		if (unit.HasMovementGlow()) {
			EntityId glow = unit.movementGlowEntity;

			em.FlagComponents.GetComponent(glow).set(ComponentFlags::AnimationEnabled);
			em.FlagComponents.GetComponent(glow).set(ComponentFlags::RenderEnabled);
		}
	}
}
void UnitTurningState::ExitState(
	UnitStateMachinChangeData& data, EntityManager& em)
{
	for (EntityId id : data.entities) {
		FlagsComponent& flags = em.FlagComponents.GetComponent(id);
		const UnitComponent& unit = em.UnitArchetype.UnitComponents.GetComponent(id);
		flags.clear(ComponentFlags::NavigationWork);
		if (unit.HasMovementGlow()) {
			EntityId glow = unit.movementGlowEntity;
			em.FlagComponents.GetComponent(glow).clear(ComponentFlags::AnimationEnabled);
			em.FlagComponents.GetComponent(glow).clear(ComponentFlags::RenderEnabled);
		}
	}
}

// ============================ Movement State ====================================

void UnitMovingState::EnterState(
	UnitStateMachinChangeData& data, EntityManager& em)
{
	for (EntityId id : data.entities) {

		const UnitComponent& unit = em.UnitArchetype.UnitComponents.GetComponent(id);
		FlagsComponent& flags = em.FlagComponents.GetComponent(id);

		em.UnitArchetype.AnimationArchetype.OrientationArchetype.AnimOrientationComponents
			.GetComponent(id).CopyArray(unit.def->Graphics->MovementAnimations);

		flags.set(ComponentFlags::NavigationWork);
		flags.set(ComponentFlags::AnimationSetChanged);
		flags.set(ComponentFlags::AnimationEnabled);

		if (unit.HasMovementGlow()) {
			EntityId glow = unit.movementGlowEntity;
			em.FlagComponents.GetComponent(glow).set(ComponentFlags::AnimationEnabled);
			em.FlagComponents.GetComponent(glow).set(ComponentFlags::RenderEnabled);
			em.FlagComponents.GetComponent(glow).set(ComponentFlags::OrientationChanged);
		}

	}
}

void UnitMovingState::ExitState(
	UnitStateMachinChangeData& data, EntityManager& em)
{
	for (EntityId id : data.entities) {
		FlagsComponent& flags = em.FlagComponents.GetComponent(id);
		const UnitComponent& unit = em.UnitArchetype.UnitComponents.GetComponent(id);
		em.MovementArchetype.MovementComponents.GetComponent(id).velocity = { 0,0 };
		flags.clear(ComponentFlags::NavigationWork);
		if (unit.HasMovementGlow()) {
			EntityId glow = unit.movementGlowEntity;
			em.FlagComponents.GetComponent(glow).clear(ComponentFlags::AnimationEnabled);
			em.FlagComponents.GetComponent(glow).clear(ComponentFlags::RenderEnabled);
		}
	}
}

// ============================ Attack State ====================================

void UnitAttackingState::EnterState(
	UnitStateMachinChangeData& data, EntityManager& em)
{
	SectionProfiler p("AttackState");

	scratch.clear();

	em.NewEntities(data.entities.size(), scratch);
	em.RenderArchetype.Archetype.AddSortedEntities(scratch);
	em.AnimationArchetype.Archetype.AddSortedEntities(scratch);
	em.MapObjectArchetype.Archetype.AddSortedEntities(scratch);
	em.TimingArchetype.Archetype.AddSortedEntities(scratch);

	int i = 0;

	for (EntityId id : data.entities) {

		const UnitComponent& unit = em.UnitArchetype.UnitComponents.GetComponent(id);
		FlagsComponent& flags = em.FlagComponents.GetComponent(id);

		em.UnitArchetype.AnimationArchetype.OrientationArchetype.AnimOrientationComponents
			.GetComponent(id).CopyArray(unit.def->Graphics->AttackAnimations);

		flags.set(ComponentFlags::AnimationSetChanged);
		flags.set(ComponentFlags::AnimationEnabled);

		if (unit.HasMovementGlow()) {
			EntityId glow = unit.movementGlowEntity;
			em.FlagComponents.GetComponent(glow).clear(ComponentFlags::AnimationEnabled);
			em.FlagComponents.GetComponent(glow).clear(ComponentFlags::RenderEnabled);
		}

		if (unit.def->Weapon->Sound.TotalClips > 0) {
			int i = std::rand() % unit.def->Weapon->Sound.TotalClips;

			em.SoundArchetype.SourceComponents.GetComponent(id).clip = unit.def->Weapon->Sound.Clips[i];
			em.SoundArchetype.SourceComponents.GetComponent(id).priority = unit.def->AudioPriority;
			em.FlagComponents.GetComponent(id).set(ComponentFlags::SoundTrigger);

			//Game::Audio.PlayClip(unit.def->Weapon->Sound.Clips[0], 1);
		}

		TimingComponent& timer = em.TimingArchetype.TimingComponents.GetComponent(id);
		TimingActionComponent& timerAction = em.TimingArchetype.ActionComponents.GetComponent(id);

		timer.NewTimer(unit.def->Weapon->Cooldown);
		timerAction.action = TimerExpiredAction::WeaponAttack;
		flags.set(ComponentFlags::UpdateTimers);

		// Particle Effect

		const auto& stateData = em.UnitArchetype.StateDataComponents.GetComponent(id);

		EntityId e = scratch[i];
		em.PositionComponents.NewComponent(e, stateData.target.position);
		em.FlagComponents.NewComponent(e).set(ComponentFlags::PositionChanged);

		EntityUtil::PlayAnimation(e, unit.def->Weapon->TargetEffect[0]);
		EntityUtil::SetRenderFromAnimationClip(e, unit.def->Weapon->TargetEffect[0], 0);
		EntityUtil::SetMapObjectBoundingBoxFromRender(e);


		EntityUtil::StartTimerMT(e, unit.def->Weapon->TargetEffect[0].GetDuration(), TimerExpiredAction::DeleteEntity);
		++i;
	}

}
void UnitAttackingState::ExitState(
	UnitStateMachinChangeData& data, EntityManager& em)
{
	for (EntityId id : data.entities) {
		FlagsComponent& flags = em.FlagComponents.GetComponent(id);
		flags.clear(ComponentFlags::UpdateTimers);
	}
}

// ============================ Death State ====================================

void UnitDeathState::EnterState(
	UnitStateMachinChangeData& data, EntityManager& em)
{

	em.UnitArchetype.Archetype.RemoveSortedEntities(data.entities);
	em.CollisionArchetype.Archetype.RemoveSortedEntities(data.entities);
	em.NavigationArchetype.Archetype.RemoveSortedEntities(data.entities);
	em.MovementArchetype.Archetype.RemoveSortedEntities(data.entities);

	for (EntityId id : data.entities) {

		const UnitComponent& unit = em.UnitArchetype.UnitComponents.GetComponent(id);

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
		const auto& clip = unit.def->Sounds.Death.Clips[i];

		em.SoundArchetype.SourceComponents.GetComponent(id).clip = clip;
		em.SoundArchetype.SourceComponents.GetComponent(id).priority = 100 + unit.def->AudioPriority;
		em.FlagComponents.GetComponent(id).set(ComponentFlags::SoundTrigger);
	}
}

void UnitDeathState::ExitState(
	UnitStateMachinChangeData& data,  EntityManager& em)
{
	EXCEPTION("Cannot exit UnitDeathState!");
}
