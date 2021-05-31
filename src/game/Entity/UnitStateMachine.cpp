#include "UnitStateMachine.h"
#include "EntityManager.h"
#include "EntityUtil.h"
#include "../Game.h"
#include "../Profiler.h"
#include "../Engine/JobSystem.h"

#include <algorithm>


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



static std::vector<EntityId> scratch;
static UnitStateMachineChangeData* _d;
static EntityManager* _e;

std::vector<IUnitState*> UnitStateMachine::States = {
	new UnitIdleState(),
	new UnitTurningState(),
	new UnitMovingState(),
	new UnitAttackingState(),
	new UnitDeathState(),
	new UnitProducingState(),
	new UnitMiningState(),
};

// ============================ Idle State ====================================

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
		auto& movement = em.MovementArchetype.MovementComponents.GetComponent(id);
		const auto& orientation = em.OrientationComponents.GetComponent(id);
		const auto& velocity = em.UnitArchetype.MovementComponents.GetComponent(id).movementSpeed;

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
		Rectangle16 targetCollider = em.CollisionArchetype.ColliderComponents.GetComponent(stateData.target.entityId).collider;

		targetCollider.position += targetPos;


		UnitComponent& unit = em.UnitArchetype.UnitComponents.GetComponent(id);
		FlagsComponent& flags = em.FlagComponents.GetComponent(id);

		uint8_t orientation = EntityUtil::GetOrientationToPosition(id, targetPos);
		em.OrientationComponents.GetComponent(id) = orientation;


		em.UnitArchetype.AnimationArchetype.OrientationArchetype.AnimOrientationComponents
			.GetComponent(id).clips = unit.def->Graphics->AttackAnimations;

		flags.set(ComponentFlags::AnimationSetChanged);
		flags.set(ComponentFlags::AnimationEnabled);
		flags.set(ComponentFlags::OrientationChanged);


		//if (unit.HasMovementGlow()) {
		//	EntityId glow = unit.movementGlowEntity;
		//	em.FlagComponents.GetComponent(glow).clear(ComponentFlags::AnimationEnabled);
		//	em.FlagComponents.GetComponent(glow).clear(ComponentFlags::RenderEnabled);
		//}

		if (unit.def->Weapon->Sound.TotalClips > 0) {
			int i = std::rand() % unit.def->Weapon->Sound.TotalClips;

			em.SoundArchetype.SourceComponents.GetComponent(id).clip = &unit.def->Weapon->Sound.Clips[i];
			em.SoundArchetype.SourceComponents.GetComponent(id).priority = unit.def->AudioPriority;
			flags.set(ComponentFlags::SoundTrigger);
		}

		UnitWeaponComponent& weapon = em.UnitArchetype.WeaponComponents.GetComponent(id);
		weapon.StartCooldown();
		
		bool kill = em.UnitArchetype.HealthComponents.GetComponent(stateData.target.entityId)
			.Reduce(weapon.damage);

		if (kill) {
			++unit.kills;
		}

		TimingComponent& timer = em.TimingArchetype.TimingComponents.GetComponent(id);
		TimingActionComponent& timerAction = em.TimingArchetype.ActionComponents.GetComponent(id);

		// TODO: from weapon component because speed can be modified
		timer.NewTimer(unit.def->Graphics->AttackAnimations[0].GetDuration());
		timerAction.action = TimerExpiredAction::WeaponAttack;
		flags.set(ComponentFlags::UpdateTimers);
		flags.set(ComponentFlags::UnitAIPaused);

	
		// Particle Effect

		targetCollider.Shrink(unit.def->Weapon->TargetEffect[0].GetFrameSize()>> 1);

		Vector2Int16 spawnAt = targetCollider.Closest(em.PositionComponents.GetComponent(id));

		EntityId e = scratch[i];

		em.TimingArchetype.TimingComponents.GetComponent(e).NewTimer(unit.def->Weapon->TargetEffect[0].GetDuration());
		em.TimingArchetype.ActionComponents.GetComponent(e).action = TimerExpiredAction::DeleteEntity;
		em.FlagComponents.NewComponent(e).set(ComponentFlags::UpdateTimers);
		em.FlagComponents.GetComponent(e).set(ComponentFlags::PositionChanged);
		em.RenderArchetype.RenderComponents.GetComponent(e).depth = 1;
		em.PositionComponents.NewComponent(e, spawnAt);

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

		FlagsComponent& flags = em.FlagComponents.GetComponent(id);

		// Clear this in order to remove unit animation change on death 
		// or just replace death animation with directional ones
		flags.clear(ComponentFlags::OrientationChanged);
		flags.clear(ComponentFlags::AnimationSetChanged);

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


// ============================ Producing State ====================================


void UnitProducingState::EnterState(UnitStateMachineChangeData& data, EntityManager& em)
{
	for (EntityId id : data.entities) {
		const UnitComponent& unit = em.UnitArchetype.UnitComponents.GetComponent(id);

		if (unit.HasMovementGlow()) {
			EntityId glow = unit.movementGlowEntity;
			em.FlagComponents.GetComponent(glow).set(ComponentFlags::AnimationEnabled);
			em.FlagComponents.GetComponent(glow).set(ComponentFlags::RenderEnabled);
		}
	}
}

void UnitProducingState::ExitState(UnitStateMachineChangeData& data, EntityManager& em)
{
	for (EntityId id : data.entities) {
		const UnitComponent& unit = em.UnitArchetype.UnitComponents.GetComponent(id);

		if (unit.HasMovementGlow()) {
			EntityId glow = unit.movementGlowEntity;
			em.FlagComponents.GetComponent(glow).clear(ComponentFlags::AnimationEnabled);
			em.FlagComponents.GetComponent(glow).clear(ComponentFlags::RenderEnabled);
		}
	}
}


// ============================  Mining State ====================================


void UnitMiningState::EnterState(UnitStateMachineChangeData& data, EntityManager& em)
{
	scratch.clear();

	em.NewEntities(data.entities.size(), scratch);
	em.RenderArchetype.Archetype.AddSortedEntities(scratch);
	em.AnimationArchetype.Archetype.AddSortedEntities(scratch);
	em.MapObjectArchetype.Archetype.AddSortedEntities(scratch);
	em.TimingArchetype.Archetype.AddSortedEntities(scratch);


	int start = 0;
	int end = data.size();
	for (int i = start; i < end; ++i) {
		EntityId id = data.entities[i];
		auto& stateData = em.UnitArchetype.StateDataComponents.GetComponent(id);
		Vector2Int16 targetPos = em.PositionComponents.GetComponent(stateData.target.entityId);
		Rectangle16 targetCollider = em.CollisionArchetype.ColliderComponents.GetComponent(stateData.target.entityId).collider;

		targetCollider.position += targetPos;


		UnitComponent& unit = em.UnitArchetype.UnitComponents.GetComponent(id);
		FlagsComponent& flags = em.FlagComponents.GetComponent(id);

		uint8_t orientation = EntityUtil::GetOrientationToPosition(id, targetPos);
		em.OrientationComponents.GetComponent(id) = orientation;


		em.UnitArchetype.AnimationArchetype.OrientationArchetype.AnimOrientationComponents
			.GetComponent(id).clips = unit.def->Graphics->AttackAnimations;

		flags.set(ComponentFlags::AnimationSetChanged);
		flags.set(ComponentFlags::AnimationEnabled);
		flags.set(ComponentFlags::OrientationChanged);


		if (unit.def->Weapon->Sound.TotalClips > 0) {
			int i = std::rand() % unit.def->Weapon->Sound.TotalClips;

			em.SoundArchetype.SourceComponents.GetComponent(id).clip = &unit.def->Weapon->Sound.Clips[i];
			em.SoundArchetype.SourceComponents.GetComponent(id).priority = unit.def->AudioPriority;
			flags.set(ComponentFlags::SoundTrigger);
		}


		TimingComponent& timer = em.TimingArchetype.TimingComponents.GetComponent(id);
		TimingActionComponent& timerAction = em.TimingArchetype.ActionComponents.GetComponent(id);

		timer.NewTimer(TimeUtil::SecondsTime(3));
		timerAction.action = TimerExpiredAction::Mining;
		flags.set(ComponentFlags::UpdateTimers);

		// Particle Effect
		targetCollider.Shrink({ 16,4 });
		Vector2Int16 spawnAt = targetCollider.Closest(em.PositionComponents.GetComponent(id));
	

		EntityId e = scratch[i];

		stateData.other.miningEffect = e;

		em.FlagComponents.NewComponent(e).set(ComponentFlags::UpdateTimers);
		em.FlagComponents.GetComponent(e).set(ComponentFlags::PositionChanged);
		em.RenderArchetype.RenderComponents.GetComponent(e).depth = 1;
		em.PositionComponents.NewComponent(e, spawnAt);

		EntityUtil::PlayAnimation(e, unit.def->Weapon->TargetEffect[0]);
		EntityUtil::SetRenderFromAnimationClip(e, unit.def->Weapon->TargetEffect[0], 0);
		EntityUtil::SetMapObjectBoundingBoxFromRender(e);
		em.AnimationArchetype.TrackerComponents.GetComponent(e).looping = true;
	}
}
void UnitMiningState::ExitState(UnitStateMachineChangeData& data, EntityManager& em)
{
	int start = 0;
	int end = data.size();

	scratch.clear();

	for (int i = start; i < end; ++i) {
		EntityId id = data.entities[i];
		FlagsComponent& flags = em.FlagComponents.GetComponent(id);
		flags.clear(ComponentFlags::UpdateTimers);

		auto& stateData = em.UnitArchetype.StateDataComponents.GetComponent(id);
		scratch.push_back(stateData.other.miningEffect);

		flags.clear(ComponentFlags::SoundTrigger);
	}

	em.DeleteEntities(scratch, false);

}
