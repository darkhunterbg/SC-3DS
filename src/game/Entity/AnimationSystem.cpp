#include "AnimationSystem.h"
#include "../Job.h"
#include "EntityManager.h"
#include "../Profiler.h"
#include <cstring>


static AnimationSystem* s;


void AnimationSystem::UpdateAnimationsJob(int start, int end) {
	AnimationData& data = s->data;

	/*
	for (int i = start; i < end; ++i) {
		const auto& anim = data.animation[i];
		const auto& tracker = data.tracker[i];
		auto& ren = *data.ren[i];
		auto& offset = *data.offset[i];

		const UnitSpriteFrame& frame = anim.clip->GetFrame(tracker.clipFrame);
		ren.hFlip = frame.hFlip;
		ren.sprite = frame.sprite.image;
		ren.shadowSprite = frame.shadowSprite.image;
		ren.colorSprite = frame.colorSprite.image;

		offset.offset = frame.offset;
		offset.shadowOffset = frame.shadowOffset;

		data.changed[i]->changed = true;
	}
	*/
}

EntityManager* e;


void AnimationSystem::GenerateAnimationsUpdatesJob(int start, int end) {
	EntityManager& em = *e;
	AnimationSystem& system = *s;

	auto data = em.AnimationArchetype.Archetype.GetEntities();

	for (int index = start; index < end; index++) {
		EntityId id = data[index];
		int i = Entity::ToIndex(id);

		auto& enable = em.AnimationArchetype.EnableComponents[i];

		if (enable.pause)
			continue;

		auto& tracker = em.AnimationArchetype.TrackerComponents[i];

		tracker.frameCountdown--;
		if (tracker.frameCountdown > 0)
			continue;

		tracker.clipFrame++;
		tracker.frameCountdown = tracker.frameTime;

		em.AnimationArchetype.ChangedComponenets[i].frameChanged = true;

		int framesCount = tracker.totalFrames;
		if (tracker.looping)
			tracker.clipFrame %= framesCount;

		enable.pause = tracker.clipFrame >= framesCount;
	}
}

void AnimationSystem::GenerateAnimationUpdates(EntityManager& em)
{
	e = &em;
	s = this;

	JobSystem::RunJob(em.AnimationArchetype.Archetype.GetEntities().Size(),
		JobSystem::DefaultJobSize, GenerateAnimationsUpdatesJob);

	data.clear();

	for (EntityId id : em.AnimationArchetype.Archetype.GetEntities()) {
		int i = Entity::ToIndex(id);

		if (em.RenderArchetype.Archetype.HasEntity(id) &&
			!em.AnimationArchetype.EnableComponents[i].pause &&
			em.AnimationArchetype.ChangedComponenets[i].frameChanged )
			// Bool component, frame changed
		{
			em.AnimationArchetype.ChangedComponenets[i].frameChanged = false;

			data.animation.push_back(em.AnimationArchetype.AnimationComponents[i]);
			data.tracker.push_back(em.AnimationArchetype.TrackerComponents[i]);
			data.ren.push_back(&em.RenderArchetype.RenderComponents[i]);
			//data.offset.push_back(&em.RenderArchetype.OffsetComponents[i]);
			data.changed.push_back(&em.EntityChangeComponents[i]);
		}
	}

}


void AnimationSystem::UpdateAnimations() {
	s = this;
	JobSystem::RunJob(data.size(), JobSystem::DefaultJobSize, UpdateAnimationsJob);
}


void AnimationSystem::SetUnitOrientationAnimations(EntityManager& em) {
	for (EntityId id : em.UnitArchetype.Archetype.GetEntities()) {
		int i = Entity::ToId(id);
		auto& orientation = em.UnitArchetype.OrientationComponents[i];

		if (orientation.changed)
		{
			orientation.changed = false;

			const auto& unit = em.UnitArchetype.UnitComponents[i];
			auto& anim = em.AnimationArchetype.AnimationComponents[i];
			auto& animEnable = em.AnimationArchetype.EnableComponents[i];
			auto& animTracker = em.AnimationArchetype.TrackerComponents[i];
			auto& changed = em.AnimationArchetype.ChangedComponenets[i];

			anim.clip = &unit.def->MovementAnimations[orientation.orientation];
			changed.frameChanged = true;
			animTracker.PlayClip(anim.clip);
			animEnable.pause = false;
		}
		else {
			//static const  Vector2Int8 zero = Vector2Int8{ 0,0 };

			//if (em.MovementArchetype.MovementComponents[i].velocity == zero) {

			//	em.AnimationArchetype.EnableComponents[i].pause = true;
			//}
		}
	}
}