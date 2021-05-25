#include "UnitStateMachine.h"
#include "EntityManager.h"
#include "EntityUtil.h"
#include "../Game.h"
#include "../Profiler.h"
#include "../Job.h"

#include <algorithm>

static std::vector<EntityId> scratch;
static UnitStateMachineChangeData* _d;
static EntityManager* _e;


// ============================ Idle State ====================================


std::array<IUnitState*, UnitStatesCount> UnitStateMachine::States = {
	new UnitIdleState(),
	new UnitTurningState(),
	new UnitMovingState(),
	new UnitAttackingState(),
	new UnitDeathState()
};

void UnitIdleState::EnterState(
	UnitStateMachineChangeData& data, EntityManager& em)
{
	for (EntityId id : data.entities) {

		const UnitComponent& unit = em.UnitArchetype.UnitComponents.GetComponent(id);

		em.UnitArchetype.AnimationArchetype.OrientationArchetype.AnimOrientationComponents
			.GetComponent(id).clips = unit.def->Graphics->IdleAnimations;

		FlagsComponent& flags = em.FlagComponents.GetComponent(id);

		flags.set(ComponentFlags::AnimationEnabled);
		flags.set(ComponentFlags::AnimationSetChanged);
	}
}
void UnitIdleState::ExitState(
	UnitStateMachineChangeData& data, EntityManager& em)
{
	// Nothing for exiting idle state
}


// ============================ Turning State ====================================


void UnitTurningState::EnterState(
	UnitStateMachineChangeData& data, EntityManager& em)
{
	for (EntityId id : data.entities) {

		const UnitComponent& unit = em.UnitArchetype.UnitComponents.GetComponent(id);

		em.UnitArchetype.AnimationArchetype.OrientationArchetype.AnimOrientationComponents
			.GetComponent(id).clips = unit.def->Graphics->IdleAnimations;

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
	UnitStateMachineChangeData& data, EntityManager& em)
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
	UnitStateMachineChangeData& data, EntityManager& em)
{
	for (EntityId id : data.entities) {

		const UnitComponent& unit = em.UnitArchetype.UnitComponents.GetComponent(id);
		FlagsComponent& flags = em.FlagComponents.GetComponent(id);

		em.UnitArchetype.AnimationArchetype.OrientationArchetype.AnimOrientationComponents
			.GetComponent(id).clips = unit.def->Graphics->MovementAnimations;

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
	UnitStateMachineChangeData& data, EntityManager& em)
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


static void  UnitAttackingEnterStateJob(int start, int end) {
	auto& data = *_d;
	auto& em = *_e;

	for (int i = start; i < end; ++i) {
		EntityId id = data.entities[i];
		const auto& stateData = em.UnitArchetype.StateDataComponents.GetComponent(id);
		Vector2Int16 targetPos = em.PositionComponents.GetComponent(stateData.target.entityId);


		const UnitComponent& unit = em.UnitArchetype.UnitComponents.GetComponent(id);
		FlagsComponent& flags = em.FlagComponents.GetComponent(id);

		uint8_t orientation = EntityUtil::GetOrientationToPosition(id, targetPos);
		em.OrientationComponents.GetComponent(id) = orientation;


		em.UnitArchetype.AnimationArchetype.OrientationArchetype.AnimOrientationComponents
			.GetComponent(id).clips = unit.def->Graphics->AttackAnimations;

		flags.set(ComponentFlags::AnimationSetChanged);
		flags.set(ComponentFlags::AnimationEnabled);
		flags.set(ComponentFlags::OrientationChanged);


		if (unit.HasMovementGlow()) {
			EntityId glow = unit.movementGlowEntity;
			em.FlagComponents.GetComponent(glow).clear(ComponentFlags::AnimationEnabled);
			em.FlagComponents.GetComponent(glow).clear(ComponentFlags::RenderEnabled);
		}

		if (unit.def->Weapon->Sound.TotalClips > 0) {
			int i = std::rand() % unit.def->Weapon->Sound.TotalClips;

			em.SoundArchetype.SourceComponents.GetComponent(id).clip = &unit.def->Weapon->Sound.Clips[i];
			em.SoundArchetype.SourceComponents.GetComponent(id).priority = unit.def->AudioPriority;
			flags.set(ComponentFlags::SoundTrigger);
		}

		UnitWeaponComponent& weapon = em.UnitArchetype.WeaponComponents.GetComponent(id);
		weapon.StartCooldown();
		em.UnitArchetype.HealthComponents.GetComponent(stateData.target.entityId).Reduce(weapon.damage);


		TimingComponent& timer = em.TimingArchetype.TimingComponents.GetComponent(id);
		TimingActionComponent& timerAction = em.TimingArchetype.ActionComponents.GetComponent(id);

		// TODO: from weapon component because speed can be modified
		timer.NewTimer(unit.def->Graphics->AttackAnimations[0].GetDuration());
		timerAction.action = TimerExpiredAction::WeaponAttack;
		flags.set(ComponentFlags::UpdateTimers);
		flags.set(ComponentFlags::UnitAIPaused);

	
		// Particle Effect

		EntityId e = scratch[i];


		em.TimingArchetype.TimingComponents.GetComponent(e).NewTimer(unit.def->Weapon->TargetEffect[0].GetDuration());
		em.TimingArchetype.ActionComponents.GetComponent(e).action = TimerExpiredAction::DeleteEntity;
		em.FlagComponents.NewComponent(e).set(ComponentFlags::UpdateTimers);
		em.FlagComponents.GetComponent(e).set(ComponentFlags::PositionChanged);

		em.PositionComponents.NewComponent(e, targetPos);

		EntityUtil::PlayAnimation(e, unit.def->Weapon->TargetEffect[0]);
		EntityUtil::SetRenderFromAnimationClip(e, unit.def->Weapon->TargetEffect[0], 0);
		EntityUtil::SetMapObjectBoundingBoxFromRender(e);

	}
}

void UnitAttackingState::EnterState(
	UnitStateMachineChangeData& data, EntityManager& em)
{
	scratch.clear();

	em.NewEntities(data.entities.size(), scratch);
	em.RenderArchetype.Archetype.AddSortedEntities(scratch);
	em.AnimationArchetype.Archetype.AddSortedEntities(scratch);
	em.MapObjectArchetype.Archetype.AddSortedEntities(scratch);
	em.TimingArchetype.Archetype.AddSortedEntities(scratch);

	_d = &data;
	_e = &em;
	JobSystem::RunJob(data.size(), JobSystem::DefaultJobSize, UnitAttackingEnterStateJob);

}
void UnitAttackingState::ExitState(
	UnitStateMachineChangeData& data, EntityManager& em)
{
	for (EntityId id : data.entities) {
		FlagsComponent& flags = em.FlagComponents.GetComponent(id);
		flags.clear(ComponentFlags::UpdateTimers);
	}
}


// ============================ Death State ====================================


void UnitDeathState::EnterState(
	UnitStateMachineChangeData& data, EntityManager& em)
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

		em.SoundArchetype.SourceComponents.GetComponent(id).clip = &clip;
		em.SoundArchetype.SourceComponents.GetComponent(id).priority = 100 + unit.def->AudioPriority;
		em.FlagComponents.GetComponent(id).set(ComponentFlags::SoundTrigger);
	}
}

void UnitDeathState::ExitState(
	UnitStateMachineChangeData& data, EntityManager& em)
{
	EXCEPTION("Cannot exit UnitDeathState!");
}
