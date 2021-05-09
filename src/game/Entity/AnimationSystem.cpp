#include "AnimationSystem.h"
#include "../Job.h"
#include "EntityManager.h"
#include "../Profiler.h"
#include <cstring>


static AnimationSystem* s;

void AnimationSystem::UpdateAnimationsJob(int start, int end) {
	AnimationData& data = s->data;

	for (int i = start; i < end; ++i) {
		const auto& anim = data.animation[i];
		const auto& tracker = data.tracker[i];
		auto& ren = *data.ren[i];
		auto& offset = *data.offset[i];

		const SpriteFrame& frame = anim.clip->GetFrame(tracker.clipFrame);
		ren.sprite = frame.sprite.image;
		ren.hFlip = frame.hFlip;
		offset.offset = frame.offset;


		if (anim.shadowClip != nullptr) {
			const SpriteFrame& shadowFrame = anim.shadowClip->GetFrame(tracker.clipFrame);
			ren.shadowSprite = shadowFrame.sprite.image;
			offset.shadowOffset = shadowFrame.offset;
		}
		else {
			//rcomp.shadowSprite.rect.size = { 0,0 };
		}
		if (anim.unitColorClip != nullptr) {
			ren.colorSprite = anim.unitColorClip->GetFrame(tracker.clipFrame).sprite.image;
		}
		else {
			ren.unitColor = 0;
		}

		data.changed[i]->changed = true;

	}
}

void AnimationSystem::GenerateAnimationUpdates(EntityManager& em)
{
	data.clear();
	int end = em.AnimationArchetype.EnableComponents.size();

	for (EntityId id : em.AnimationArchetype.Archetype.GetEntities()) {
		int i = Entity::ToIndex(id);

		auto& enable = em.AnimationArchetype.EnableComponents[i];

		if (enable.pause)
			continue;

		auto& tracker = em.AnimationArchetype.TrackerComponents[i];

		tracker.clipFrame++;

		int framesCount = tracker.totalFrames;
		if (tracker.looping)
			tracker.clipFrame %= framesCount;

		if (tracker.clipFrame < framesCount &&
			em.RenderArchetype.Archetype.HasEntity(id))
		{
			data.animation.push_back(em.AnimationArchetype.AnimationComponents[i]);
			data.tracker.push_back(em.AnimationArchetype.TrackerComponents[i]);
			data.ren.push_back(&em.RenderArchetype.RenderComponents[i]);
			data.offset.push_back(&em.RenderArchetype.OffsetComponents[i]);
			data.changed.push_back(&em.EntityChangeComponents[i]);
		}
		else {
			enable.pause = true;
		}
	}
}



void AnimationSystem::UpdateAnimations() {

	s = this;

	JobSystem::RunJob(data.size(), JobSystem::DefaultJobSize, UpdateAnimationsJob);
}
