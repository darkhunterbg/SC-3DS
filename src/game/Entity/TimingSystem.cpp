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
		unsigned total = entities.size();
		em.DeleteEntities(entities, true);
		em.NewEntities(total, scratch);
		em.RenderArchetype.Archetype.AddEntities(scratch, true);
		em.AnimationArchetype.Archetype.AddEntities(scratch, true);

		for (unsigned item = 0; item < total; ++item)
		{
			EntityId id = scratch[item];
			EntityId old = entities[item];

			const auto def = em.UnitArchetype.UnitComponents.GetComponent(old).def;
			const auto& clip = def->Graphics->Remnants.Clip;

			em.PositionComponents.NewComponent(id) = em.PositionComponents.GetComponent(old);
			em.FlagComponents.NewComponent(id).set(ComponentFlags::PositionChanged);
		
			EntityUtil::SetRenderFromAnimationClip(id, clip, 0);
			em.RenderArchetype.RenderComponents.GetComponent(id).depth = def->Graphics->Remnants.Depth;
			em.RenderArchetype.BoundingBoxComponents.GetComponent(id)
				.SetCenter(em.PositionComponents.GetComponent(id));

			EntityUtil::StartTimer(id, clip.GetDuration(), TimerExpiredAction::DeleteEntity);
			EntityUtil::PlayAnimation(id, clip);
		}

		scratch.clear();
	}
}

void TimingSystem::DeleteEntities(std::vector<EntityId>& entities, EntityManager& em) {
	if (entities.size() > 0) {
		em.DeleteEntitiesSorted(entities);
	}
}