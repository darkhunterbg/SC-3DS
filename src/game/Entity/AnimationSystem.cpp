#include "AnimationSystem.h"
#include "../Job.h"
#include "EntityManager.h"
#include "../Profiler.h"
#include <cstring>


static AnimationSystem* s;
static EntityManager* e;

void AnimationSystem::UpdateUnitAnimationsJob(int start, int end) {
	UnitAnimationData& data = s->unitData;

	for (int i = start; i < end; ++i) {
		const auto& anim = data.animation[i];
		const auto& tracker = data.tracker[i];
		auto& ren = *data.ren[i];
		auto& offset = *data.offset[i];
		auto& flags = *data.flags[i];

		if (tracker.clipFrame < 0)
			continue;

		const UnitSpriteFrame& frame = anim.clip->GetFrame(tracker.clipFrame);

		ren.hFlip = frame.hFlip;
		ren.sprite = frame.sprite.image;
		ren.shadowSprite = frame.shadowSprite.image;
		ren.colorSprite = frame.colorSprite.image;

		offset.offset = frame.offset;
		offset.shadowOffset = frame.shadowOffset;

		flags.set(ComponentFlags::UnitRenderChanged);
	}
	
}
void AnimationSystem::UpdateAnimationsJob(int start, int end) {
	AnimationData& data = s->data;

	for (int i = start; i < end; ++i) {
		const auto& anim = data.animation[i];
		const auto& tracker = data.tracker[i];
		auto& ren = *data.ren[i];
		auto& offset = *data.offset[i];
		auto& flags = *data.flags[i];

		const SpriteFrame& frame = anim.clip->GetFrame(tracker.clipFrame);
		ren.hFlip = frame.hFlip;
		ren.sprite = frame.sprite.image;

		offset = frame.offset;

		flags.set(ComponentFlags::RenderChanged);
	}
}

void AnimationSystem::UpdateAnimations() {
	s = this;
	JobSystem::RunJob(unitData.size(), JobSystem::DefaultJobSize, UpdateUnitAnimationsJob);
	JobSystem::RunJob(data.size(), JobSystem::DefaultJobSize, UpdateAnimationsJob);
}

void AnimationSystem::TickUnitAnimationsJob(int start, int end) {
	EntityManager& em = *e;

	auto data = em.UnitArchetype.AnimationArchetype.Archetype.GetEntities();

	for (int index = start; index < end; index++) {
		EntityId id = data[index];
		int i = Entity::ToIndex(id);

		auto& f = em.FlagComponents[i];

		if (!f.test(ComponentFlags::AnimationEnabled))
			continue;

		auto& tracker = em.UnitArchetype.AnimationArchetype.TrackerComponents[i];

		tracker.frameCountdown--;
		if (tracker.frameCountdown > 0)
			continue;

		tracker.clipFrame++;
		tracker.frameCountdown = tracker.frameTime;

		f.set(ComponentFlags::UnitAnimationFrameChanged);

		if (tracker.looping)
			tracker.clipFrame %= tracker.totalFrames;


		f.set(ComponentFlags::AnimationEnabled, tracker.clipFrame < tracker.totalFrames);
	}
}
void AnimationSystem::TickAnimationsJob(int start, int end) {
	EntityManager& em = *e;

	auto data = em.AnimationArchetype.Archetype.GetEntities();

	for (int index = start; index < end; index++) {
		EntityId id = data[index];
		int i = Entity::ToIndex(id);

		auto& f = em.FlagComponents[i];

		if (!f.test(ComponentFlags::AnimationEnabled))
			continue;

		auto& tracker = em.AnimationArchetype.TrackerComponents[i];

		tracker.frameCountdown--;
		if (tracker.frameCountdown > 0)
			continue;

		tracker.clipFrame++;
		tracker.frameCountdown = tracker.frameTime;

		f.set(ComponentFlags::AnimationFrameChanged);

		int framesCount = tracker.totalFrames;
		if (tracker.looping)
			tracker.clipFrame %= framesCount;

		f.set(ComponentFlags::AnimationEnabled, tracker.clipFrame < framesCount);
	}
}
void AnimationSystem::TickAnimations(EntityManager& em)
{
	e = &em;
	s = this;

	JobSystem::RunJob(em.AnimationArchetype.Archetype.GetEntities().Size(),
		JobSystem::DefaultJobSize, TickAnimationsJob);
	JobSystem::RunJob(em.UnitArchetype.AnimationArchetype.Archetype.GetEntities().Size(),
		JobSystem::DefaultJobSize, TickUnitAnimationsJob);

	data.clear();
	unitData.clear();
	
	for (EntityId id : em.AnimationArchetype.Archetype.GetEntities()) {
		int i = Entity::ToIndex(id);
		auto& f = em.FlagComponents[i];

		if (f.test(ComponentFlags::AnimationEnabled))
		{
			if (f.test(ComponentFlags::AnimationFrameChanged)) {
				f.clear(ComponentFlags::AnimationFrameChanged);

				if (em.RenderArchetype.Archetype.HasEntity(id)) {
					data.animation.push_back(em.AnimationArchetype.AnimationComponents[i]);
					data.tracker.push_back(em.AnimationArchetype.TrackerComponents[i]);
					data.ren.push_back(&em.RenderArchetype.RenderComponents[i]);
					data.offset.push_back(&em.RenderArchetype.OffsetComponents[i]);
					data.flags.push_back(&f);
				}
			}
		}
	}
	

	for (EntityId id : em.UnitArchetype.AnimationArchetype.Archetype.GetEntities()) {
		int i = Entity::ToIndex(id);
		auto& f = em.FlagComponents[i];

		if (f.test(ComponentFlags::AnimationEnabled))
		{
			if (f.test(ComponentFlags::UnitAnimationFrameChanged)) {
				f.clear(ComponentFlags::UnitAnimationFrameChanged);

				if (em.UnitArchetype.RenderArchetype.Archetype.HasEntity(id)) {
					unitData.animation.push_back(em.UnitArchetype.AnimationArchetype.AnimationComponents[i]);
					unitData.tracker.push_back(em.UnitArchetype.AnimationArchetype.TrackerComponents[i]);
					unitData.ren.push_back(&em.UnitArchetype.RenderArchetype.RenderComponents[i]);
					unitData.offset.push_back(&em.UnitArchetype.RenderArchetype.OffsetComponents[i]);
					unitData.flags.push_back(&f);
				}
			}
		}
	}
}

void AnimationSystem::SetUnitOrientationAnimations(EntityManager& em) {
	for (EntityId id : em.UnitArchetype.Archetype.GetEntities()) {
		int i = Entity::ToId(id);
		auto& f = em.FlagComponents[i];

		if (f.test(ComponentFlags::UnitOrientationChanged))
		{
			f.clear(ComponentFlags::UnitOrientationChanged);

			const auto& unit = em.UnitArchetype.UnitComponents[i];
			auto& anim = em.UnitArchetype.AnimationArchetype.AnimationComponents[i];
			auto& animTracker = em.UnitArchetype.AnimationArchetype.TrackerComponents[i];
			const auto& orientation = em.UnitArchetype.OrientationComponents[i];

			anim.clip = &unit.def->Graphics->MovementAnimations[orientation];
			f.set(ComponentFlags::AnimationFrameChanged);
			animTracker.PlayClip(anim.clip);
			f.set(ComponentFlags::AnimationEnabled);
		}
		else {
			//static const  Vector2Int8 zero = Vector2Int8{ 0,0 };

			//if (em.MovementArchetype.MovementComponents[i].velocity == zero) {

			//	em.AnimationArchetype.EnableComponents[i].pause = true;
			//}
		}
	}
}