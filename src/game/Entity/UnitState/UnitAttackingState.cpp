#include "UnitAttackingState.h"
#include "../EntityManager.h"
#include "../EntityUtil.h"
#include "../../Engine/JobSystem.h"


static std::vector<EntityId> scratch;
static UnitStateMachineChangeData* _d;
static EntityManager* _e;



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


		if (unit.def->Art.GetSprite().GetAnimation(AnimationType::GroundAttackRepeat))
			EntityUtil::PlayAnimation(id, *unit.def->Art.GetSprite().GetAnimation(AnimationType::GroundAttackRepeat));

		flags.set(ComponentFlags::AnimationEnabled);
		flags.set(ComponentFlags::OrientationChanged);


		//if (unit.HasMovementGlow()) {
		//	EntityId glow = unit.movementGlowEntity;
		//	em.FlagComponents.GetComponent(glow).clear(ComponentFlags::AnimationEnabled);
		//	em.FlagComponents.GetComponent(glow).clear(ComponentFlags::RenderEnabled);
		//}

		//if (unit.def->Weapon->Sound.TotalClips > 0) {
		//	int i = std::rand() % unit.def->Weapon->Sound.TotalClips;

		//	em.SoundArchetype.SourceComponents.GetComponent(id).clip = &unit.def->Weapon->Sound.Clips[i];
		//	em.SoundArchetype.SourceComponents.GetComponent(id).priority = unit.def->AudioPriority;
		//	flags.set(ComponentFlags::SoundTrigger);
		//}

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
		//timer.NewTimer(unit.def->Graphics->AttackAnimations[0].GetDuration());
		timerAction.action = TimerExpiredAction::WeaponAttack;
		flags.set(ComponentFlags::UpdateTimers);
		flags.set(ComponentFlags::UnitAIPaused);


		// Particle Effect

		//targetCollider.Shrink(unit.def->Weapon->TargetEffect[0].GetFrameSize()>> 1);

		Vector2Int16 spawnAt = targetCollider.Closest(em.PositionComponents.GetComponent(id));

		EntityId e = scratch[i];

		//em.TimingArchetype.TimingComponents.GetComponent(e).NewTimer(unit.def->Weapon->TargetEffect[0].GetDuration());
		em.TimingArchetype.ActionComponents.GetComponent(e).action = TimerExpiredAction::DeleteEntity;
		em.FlagComponents.NewComponent(e).set(ComponentFlags::UpdateTimers);
		em.FlagComponents.GetComponent(e).set(ComponentFlags::PositionChanged);
		em.RenderArchetype.RenderComponents.GetComponent(e).depth = 1;
		em.PositionComponents.NewComponent(e, spawnAt);

		//EntityUtil::PlayAnimation(e, unit.def->Weapon->TargetEffect[0]);
		//EntityUtil::SetRenderFromAnimationClip(e, unit.def->Weapon->TargetEffect[0], 0);
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