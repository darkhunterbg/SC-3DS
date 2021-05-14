#include "TimingSystem.h"
#include "EntityManager.h"
#include "../Profiler.h"



void TimingSystem::UpdateTimers(EntityManager& em)
{
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

	UnitRemnantsThenDelete(GetActionEntityTable(TimerExpiredAction::UnitRemnantsThenDelete), em);
	DeleteEntities(GetActionEntityTable(TimerExpiredAction::DeleteEntity), em);
}

void TimingSystem::UnitRemnantsThenDelete(std::vector<EntityId>& entities, EntityManager& em) {
	if (entities.size() > 0) {
		//em.DeleteEntities(entities);
		for (EntityId id : entities) {
			int i = Entity::ToIndex(id);
			auto def = em.UnitArchetype.UnitComponents[i].def;
			em.DeleteEntity(id);

			auto e = em.NewEmptyObject(em.PositionComponents[i]);
			em.RenderArchetype.Archetype.AddEntity(e);
			em.RenderArchetype.RenderComponents.NewComponent(e).depth = def->Graphics->Remnants.Depth;
			em.RenderArchetype.RenderComponents.GetComponent(e).SetSpriteFrame(def->Graphics->Remnants.Clip.GetFrame(0));
			em.RenderArchetype.OffsetComponents.NewComponent(e) = def->Graphics->Remnants.Clip.GetFrame(0).offset;
			em.RenderArchetype.DestinationComponents.NewComponent(e) = def->Graphics->Remnants.Clip.GetFrame(0).offset + em.PositionComponents[i];
			em.RenderArchetype.BoundingBoxComponents.NewComponent(e) = em.UnitArchetype.RenderArchetype.BoundingBoxComponents
				.GetComponent(id);
			em.StartTimer(e, def->Graphics->Remnants.Clip.GetDuration() + 1, TimerExpiredAction::DeleteEntity);
			em.PlayAnimation(e, def->Graphics->Remnants.Clip);
		}
	}
}

void TimingSystem::DeleteEntities(std::vector<EntityId>& entities, EntityManager& em) {
	em.DeleteEntities(entities);
}