#include "TimingSystem.h"
#include "EntityManager.h"
#include "EntityUtil.h"

#include "../Profiler.h"



void TimingSystem::UpdateTimers(EntityManager& em)
{
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

	UnitRemnantsThenDelete(GetActionEntityTable(TimerExpiredAction::UnitRemnantsThenDelete), em);
	DeleteEntities(GetActionEntityTable(TimerExpiredAction::DeleteEntity), em);

	for (auto& t : actionsTable)
		t.entities.clear();
}

void TimingSystem::UnitRemnantsThenDelete(std::vector<EntityId>& entities, EntityManager& em) {
	if (entities.size() > 0) {
		em.ClearEntitiesArchetypes(entities);
		em.RenderArchetype.Archetype.AddEntities(entities, true);
		em.AnimationArchetype.Archetype.AddEntities(entities, true);

		for (EntityId id : entities) {
			int i = Entity::ToIndex(id);

			const auto def = em.UnitArchetype.UnitComponents[i].def;
			const auto& clip = def->Graphics->Remnants.Clip;

			em.FlagComponents.NewComponent(id);
			EntityUtil::SetRenderFromAnimationClip(id, clip, 0);
			em.RenderArchetype.BoundingBoxComponents.GetComponent(id) =
				em.UnitArchetype.RenderArchetype.BoundingBoxComponents.GetComponent(id);
			em.RenderArchetype.RenderComponents.GetComponent(id).depth = def->Graphics->Remnants.Depth;
			EntityUtil::StartTimer(id, clip.GetDuration(), TimerExpiredAction::DeleteEntity);
			EntityUtil::PlayAnimation(id, clip);
		}
	}
}

void TimingSystem::DeleteEntities(std::vector<EntityId>& entities, EntityManager& em) {
	if (entities.size() > 0) {
		em.DeleteEntities(entities);
	}
}