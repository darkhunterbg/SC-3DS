#include "UnitStateMachine.h"
#include "EntityManager.h"
#include "EntityUtil.h"
#include "../Game.h"
#include "../Profiler.h"
#include "../Engine/JobSystem.h"
#include "../Debug.h"
#include "../Data/SoundSetDef.h"

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

std::vector<IUnitState*> UnitStateMachine::States = {
	new UnitIdleState(),
	new UnitTurningState(),
	new UnitMovingState(),
	new UnitAttackingState(),
	new UnitDeathState(),
	new UnitProducingState(),
	new UnitMiningState(),
	new UnitAttackLoopState(),
};

static inline void SetAnimationIfAvaiable(EntityId id, EntityManager& em, AnimationType animation) {

	const UnitComponent& unit = em.UnitArchetype.UnitComponents.GetComponent(id);
	const AnimClipDef* anim = unit.def->Art.GetSprite().GetAnimation(animation);
	if (anim) EntityUtil::PlayAnimation(id, *anim);
}




// ============================ Idle State ====================================

void UnitIdleState::EnterState(
	UnitStateMachineChangeData& data, EntityManager& em)
{
	for (EntityId id : data.entities) {
		SetAnimationIfAvaiable(id, em, AnimationType::Init);
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
		SetAnimationIfAvaiable(id, em, AnimationType::Init);
	}
}
void UnitTurningState::ExitState(
	UnitStateMachineChangeData& data, EntityManager& em)
{
	for (EntityId id : data.entities) {
		FlagsComponent& flags = em.FlagComponents.GetComponent(id);
		flags.clear(ComponentFlags::NavigationWork);
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

		SetAnimationIfAvaiable(id, em, AnimationType::Walking);

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
		em.MovementArchetype.MovementComponents.GetComponent(id).velocity = { 0,0 };
		flags.clear(ComponentFlags::NavigationWork);
	}
}


// ============================ Attack State ====================================

void UnitAttackingState::EnterState(
	UnitStateMachineChangeData& data, EntityManager& em)
{
	for (EntityId id : data.entities) {
		const UnitComponent& unit = em.UnitArchetype.UnitComponents.GetComponent(id);
		const AnimClipDef* anim = unit.def->Art.GetSprite().GetAnimation(AnimationType::GroundAttackInit);

		const auto& stateData = em.UnitArchetype.StateDataComponents.GetComponent(id);
		Vector2Int16 targetPos = em.PositionComponents.GetComponent(stateData.target.entityId);
		Rectangle16 targetCollider = em.CollisionArchetype.ColliderComponents.GetComponent(stateData.target.entityId).collider;

		targetCollider.position += targetPos;
		uint8_t orientation = EntityUtil::GetOrientationToPosition(id, targetPos);
		em.OrientationComponents.GetComponent(id) = orientation;

		if (anim) {
			EntityUtil::PlayAnimation(id, *anim);
			em.FlagComponents.GetComponent(id).set(ComponentFlags::UnitAIWaitForAnimation);
		}
		else
		{
			em.UnitArchetype.StateComponents.GetComponent(id) = UnitState::AttackLoop;
			em.FlagComponents.GetComponent(id).set(ComponentFlags::UnitStateChanged);
		}
	}
}
void UnitAttackingState::ExitState(
	UnitStateMachineChangeData& data, EntityManager& em)
{

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
		SetAnimationIfAvaiable(id, em, AnimationType::Death);
		em.FlagComponents.GetComponent(id).clear(ComponentFlags::RenderShadows);
		const UnitDef& def = *em.UnitArchetype.UnitComponents[id].def;
		const auto sound = def.Sounds.GetDeathSound();
		if (sound != nullptr)
		{
			int r = std::rand() % sound->GetAudioClips().Size();
			em.SoundArchetype.SourceComponents.GetComponent(id).clip = &sound->GetAudioClip(r);
			em.FlagComponents.GetComponent(id).set(ComponentFlags::SoundTrigger);
		}

		em.TimingArchetype.TimingComponents.GetComponent(id).NewTimer(TimeUtil::SecondsTime(0.5f));
		em.TimingArchetype.ActionComponents.GetComponent(id).action = TimerExpiredAction::DeleteEntity;
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
		SetAnimationIfAvaiable(id, em, AnimationType::IsWorking);
	}
}

void UnitProducingState::ExitState(UnitStateMachineChangeData& data, EntityManager& em)
{

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


		//em.UnitArchetype.AnimationArchetype.OrientationArchetype.AnimOrientationComponents
		//	.GetComponent(id).clips = unit.def->Graphics->AttackAnimations;

		flags.set(ComponentFlags::AnimationEnabled);
		flags.set(ComponentFlags::OrientationChanged);


		//if (unit.def->Weapon->Sound.TotalClips > 0) {
		//	int i = std::rand() % unit.def->Weapon->Sound.TotalClips;

		//	em.SoundArchetype.SourceComponents.GetComponent(id).clip = &unit.def->Weapon->Sound.Clips[i];
		//	em.SoundArchetype.SourceComponents.GetComponent(id).priority = unit.def->AudioPriority;
		//	flags.set(ComponentFlags::SoundTrigger);
		//}


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

		//EntityUtil::PlayAnimation(e, unit.def->Weapon->TargetEffect[0]);
		//EntityUtil::SetRenderFromAnimationClip(e, unit.def->Weapon->TargetEffect[0], 0);
		//EntityUtil::SetMapObjectBoundingBoxFromRender(e);
		//em.AnimationArchetype.TrackerComponents.GetComponent(e).looping = true;
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

// ============================ Attack Loop State =====================================

void UnitAttackLoopState::EnterState(UnitStateMachineChangeData& data, EntityManager& em) {
	for (EntityId id : data.entities) {
		SetAnimationIfAvaiable(id, em, AnimationType::GroundAttackRepeat);

		em.UnitArchetype.WeaponComponents.GetComponent(id).StartCooldown();
		em.FlagComponents.GetComponent(id).set(ComponentFlags::UnitAIWaitForAnimation);
	}
}
void UnitAttackLoopState::ExitState(UnitStateMachineChangeData& data, EntityManager& em) {}