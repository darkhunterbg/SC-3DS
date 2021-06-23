#include "UnitDeathState.h"
#include "../EntityManager.h"
#include "../EntityUtil.h"
#include "../../Debug.h"


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

		if (unit.def->Art.GetSprite().GetAnimation(AnimationType::Death))
			EntityUtil::PlayAnimation(id, *unit.def->Art.GetSprite().GetAnimation(AnimationType::Death));

		//if (unit.def->Graphics->HasDeathAnimation()) {
		//	EntityUtil::PlayAnimation(id, unit.def->Graphics->DeathAnimation);
		//	EntityUtil::StartTimer(id, unit.def->Graphics->DeathAnimation.GetDuration(), TimerExpiredAction::UnitDeathAfterEffect);
		//}
		//else {
		//	EntityUtil::StartTimer(id, 1, TimerExpiredAction::UnitDeathAfterEffect);
		//}

		//int i = std::rand() % unit.def->Sounds.Death.TotalClips;
		//const auto& clip = unit.def->Sounds.Death.Clips[i];

		//em.SoundArchetype.SourceComponents.GetComponent(id).clip = &clip;
		//em.SoundArchetype.SourceComponents.GetComponent(id).priority = 100 + unit.def->AudioPriority;
		//em.FlagComponents.GetComponent(id).set(ComponentFlags::SoundTrigger);
	}
}

void UnitDeathState::ExitState(
	UnitStateMachineChangeData& data, EntityManager& em)
{
	EXCEPTION("Cannot exit UnitDeathState!");
}
