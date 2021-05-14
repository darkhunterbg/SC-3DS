#include "TimingSystem.h"
#include "EntityManager.h"
#include "../Profiler.h"


static std::vector<EntityId> expiredTimerEntities;
static std::vector<TimerExpiredAction> expiredTimerActions;

void TimingSystem::UpdateTimers(EntityManager& em)
{
	SectionProfiler p("UpdateTimers");

	expiredTimerEntities.clear();
	expiredTimerActions.clear();

	for (EntityId id : em.TimingArchetype.Archetype.GetEntities()) {
		int i = Entity::ToIndex(id);

		auto& flags = em.FlagComponents[i];
		if (!flags.test(ComponentFlags::UpdateTimers))
			continue;

		auto& timing = em.TimingArchetype.TimingComponents[i];

		for (uint8_t t = 0; t < timing.activeTimers; ++t) {
			--timing.timers[t];
			if (timing.timers[t] == 0) {

				expiredTimerEntities.push_back(id);
				expiredTimerActions.push_back(timing.actions[t]);

				--timing.activeTimers;
				--t;
			}
		}

		flags.set(ComponentFlags::UpdateTimers, timing.activeTimers);
	}
}



void TimingSystem::ApplyTimerActions(EntityManager& em) {
	SectionProfiler p("ApplyTimers");

	em.DeleteEntities(expiredTimerEntities);

	// Todo order based on timer priority, split into groups and execute foreach (death should happen last)
	/*for (const auto& timer : expiredTimers) {
		EntityId id = timer.id;*/
		//int i = Entity::ToIndex(id);

		/*switch (timer.action)
		{
		case TimerExpiredAction::DeleteEntity: {
			em.DeleteEntity(id);
			break;
		}

		default:
			break;
		}
	}*/
}