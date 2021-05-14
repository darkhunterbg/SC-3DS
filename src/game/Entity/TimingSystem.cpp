#include "TimingSystem.h"
#include "EntityManager.h"
#include "../Profiler.h"



void TimingSystem::UpdateTimers(EntityManager& em)
{
	SectionProfiler p("UpdateTimers");

	for (auto& t : actionsTable)
		t.entities.clear();

	for (EntityId id : em.TimingArchetype.Archetype.GetEntities()) {
		int i = Entity::ToIndex(id);

		auto& f = em.FlagComponents[i];
		if (!f.test(ComponentFlags::UpdateTimers))
			continue;

		auto& t = em.TimingArchetype.TimingComponents[i];
		--t.timer;
		if (t.timer == 0) {
			auto& a = em.TimingArchetype.ActionComponents[i];
			GetActionEntityTable(a.action).push_back(id);

			if (t.nextTimer > 0) {
				t.timer = t.nextTimer;
			}
			else {
				f.clear(ComponentFlags::UpdateTimers);
			}
		}
	}
}



void TimingSystem::ApplyTimerActions(EntityManager& em) {
	SectionProfiler p("ApplyTimers");

	DeleteEntities(GetActionEntityTable(TimerExpiredAction::DeleteEntity), em);
}

void TimingSystem::DeleteEntities( std::vector<EntityId>& entities, EntityManager& em) {
	em.DeleteEntities(entities);
}