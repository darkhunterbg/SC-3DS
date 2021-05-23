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

		flags.set(ComponentFlags::RenderChanged);
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

void AnimationSystem::UpdateAnimations(EntityManager& em) {
	data.clear();
	unitData.clear();

	s = this;

	for (EntityId id : em.AnimationArchetype.Archetype.GetEntities()) {
		int i = Entity::ToIndex(id);
		auto& f = em.FlagComponents[i];

		if (f.test(ComponentFlags::AnimationEnabled))
		{
			if (f.test(ComponentFlags::AnimationFrameChanged)) {
				f.clear(ComponentFlags::AnimationFrameChanged);

				data.animation.push_back(em.AnimationArchetype.AnimationComponents[i]);
				data.tracker.push_back(em.AnimationArchetype.TrackerComponents[i]);
				data.ren.push_back(&em.RenderArchetype.RenderComponents[i]);
				data.offset.push_back(&em.RenderArchetype.OffsetComponents[i]);
				data.flags.push_back(&f);

			}
		}
	}

	for (EntityId id : em.UnitArchetype.AnimationArchetype.Archetype.GetEntities()) {
		int i = Entity::ToIndex(id);
		auto& f = em.FlagComponents[i];

		if (f.test(ComponentFlags::AnimationEnabled))
		{
			if (f.test(ComponentFlags::AnimationFrameChanged)) {
				f.clear(ComponentFlags::AnimationFrameChanged);

				unitData.animation.push_back(em.UnitArchetype.AnimationArchetype.AnimationComponents[i]);
				unitData.tracker.push_back(em.AnimationArchetype.TrackerComponents[i]);
				unitData.ren.push_back(&em.UnitArchetype.RenderArchetype.RenderComponents[i]);
				unitData.offset.push_back(&em.UnitArchetype.RenderArchetype.OffsetComponents[i]);
				unitData.flags.push_back(&f);

			}
		}
	}

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

		auto& tracker = em.AnimationArchetype.TrackerComponents[i];

		tracker.frameCountdown--;
		if (tracker.frameCountdown > 0)
			continue;

		tracker.clipFrame++;
		tracker.frameCountdown = tracker.frameTime;

		f.set(ComponentFlags::AnimationFrameChanged);

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

	JobSystem::RunJob(em.AnimationArchetype.Archetype.GetEntities().size(),
		JobSystem::DefaultJobSize, TickAnimationsJob);
	JobSystem::RunJob(em.UnitArchetype.AnimationArchetype.Archetype.GetEntities().size(),
		JobSystem::DefaultJobSize, TickUnitAnimationsJob);
}



void AnimationSystem::UpdateAnimationsForOrientation(EntityManager& entityManager) {

	for (EntityId id : entityManager.AnimationArchetype.OrientationArchetype.Archetype.GetEntities()) {
		FlagsComponent& flags = entityManager.FlagComponents.GetComponent(id);
		if (flags.test(ComponentFlags::OrientationChanged) || flags.test(ComponentFlags::AnimationSetChanged)) {
			flags.clear(ComponentFlags::OrientationChanged);
			flags.clear(ComponentFlags::AnimationSetChanged);

			const auto& animOrin = entityManager.AnimationArchetype.OrientationArchetype
				.AnimOrientationComponents.GetComponent(id);

			auto& anim = entityManager.AnimationArchetype.AnimationComponents.GetComponent(id);
			auto& animTrack = entityManager.AnimationArchetype.TrackerComponents.GetComponent(id);
			uint8_t orientation = entityManager.OrientationComponents.GetComponent(id);
			anim.clip = animOrin.clips[orientation];
			animTrack.PlayClip(anim.clip);

			flags.set(ComponentFlags::AnimationFrameChanged);
		}
	}

	for (EntityId id : entityManager.UnitArchetype.AnimationArchetype.OrientationArchetype
		.Archetype.GetEntities()) {
		FlagsComponent& flags = entityManager.FlagComponents.GetComponent(id);
		if (flags.test(ComponentFlags::OrientationChanged) || flags.test(ComponentFlags::AnimationSetChanged)) {

			flags.clear(ComponentFlags::OrientationChanged);
			flags.clear(ComponentFlags::AnimationSetChanged);

			const auto& animOrin = entityManager.UnitArchetype.AnimationArchetype
				.OrientationArchetype.AnimOrientationComponents.GetComponent(id);

			auto& anim = entityManager.UnitArchetype.AnimationArchetype.AnimationComponents.GetComponent(id);
			auto& animTrack = entityManager.AnimationArchetype.TrackerComponents.GetComponent(id);
			uint8_t orientation = entityManager.OrientationComponents.GetComponent(id);
			anim.clip = animOrin.clips[orientation];
			animTrack.PlayClip(anim.clip);

			flags.set(ComponentFlags::AnimationFrameChanged);
		}
	}
}