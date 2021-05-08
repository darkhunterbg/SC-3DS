#include "AnimationSystem.h"
#include "../Job.h"
#include "EntityManager.h"
#include <cstring>


static AnimationArchetype* a;

static void UpdateAnims(int start, int end) {
	AnimationArchetype& archetype = *a;


	for (int i = start; i < end; ++i) {
		const auto& anim = archetype.animation[i];
		const auto& tracker = archetype.tracker[i];
		auto& ren = *archetype.ren[i];
		auto& offset = *archetype.offset[i];

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

		archetype.changed[i]->changed = true;

	}
}

void AnimationSystem::GenerateAnimationUpdates(EntityManager& em)
{
	archetype.clear();
	int end = em.AnimationEnableComponents.size();
	int start = 0;
	for (int i = start; i < end; ++i) {
		if (em.AnimationEnableComponents[i].pause)
			continue;

		auto& tracker = em.AnimationTrackerComponents[i];
		tracker.frameCountdown--;
		if (tracker.frameCountdown == 0) {
			tracker.clipFrame++;

			tracker.frameCountdown = tracker.frameDuration;
			int framesCount = tracker.totalFrames;
			if (tracker.looping)
				tracker.clipFrame %= framesCount;

			if (tracker.clipFrame < framesCount)
			{
				archetype.animation.push_back(em.AnimationComponents[i]);
				archetype.tracker.push_back(em.AnimationTrackerComponents[i]);
				archetype.ren.push_back(&em.RenderComponents[i]);
				archetype.offset.push_back(&em.RenderOffsetComponents[i]);
				archetype.changed.push_back(&em.EntityChangeComponents[i]);
			}
		}
	}
}


void AnimationSystem::UpdateAnimations() {
	a = &archetype;
	JobSystem::RunJob(archetype.size(), JobSystem::DefaultJobSize, UpdateAnims);
}
