#include "AnimationSystem.h"
#include "../Data/AssetDataDefs.h"
#include "EntityManager.h"
#include "EntityUtil.h"

#include "AnimationPlayer.h"
#include "../Profiler.h"

void AnimationSystem::TickAnimations(EntityManager& em)
{
	animData.clear();

	for (EntityId id : em.AnimationArchetype.Archetype.GetEntities()) {
		const FlagsComponent& flags = em.FlagComponents.GetComponent(id);
		if (flags.test(ComponentFlags::AnimationEnabled)) {
			AnimationStateComponent& state = em.AnimationArchetype.StateComponents.GetComponent(id);

			if (state.wait > 0)
				--state.wait;


			if (state.wait == 0) {
				animData.entities.push_back(id);

				animData.components.push_back(em.AnimationArchetype.AnimationComponents.GetComponent(id));

				animData.states.push_back(&em.AnimationArchetype.StateComponents.GetComponent(id));
			}
		}
	}
}

void AnimationSystem::RunAnimations(EntityManager& em)
{
	//SectionProfiler p("RunAnimations");

	int start = 0;
	int end = animData.size();

	for (int i = start; i < end; ++i) {
		EntityId id = animData.entities[i];

		auto& state = *animData.states[i];
		const auto& anim = animData.components[i];

		AnimationPlayer::RunAnimation(id, anim, state, em);
	}
}

