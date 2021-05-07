#include "AnimationSystem.h"
#include "../Job.h"
#include <cstring>


static AnimationArchetype* a;

static void UpdateAnims(int start, int end) {
	AnimationArchetype& archetype = *a;


	for (int i = start; i < end; ++i) {
		auto& anim = *archetype.animation[i];
		anim.frameCountdown--;
		if (anim.frameCountdown == 0) {
			anim.clipFrame++;

			anim.frameCountdown = anim.clip->frameDuration;
			int framesCount = anim.clip->GetFrameCount();
			if (anim.clip->looping)
				anim.clipFrame %= framesCount;


			if (anim.clipFrame < framesCount)
			{
				auto& ren = *archetype.ren[i];
				auto& offset = *archetype.offset[i];

				const SpriteFrame& frame = anim.clip->GetFrame(anim.clipFrame);
				ren.sprite = frame.sprite.image;
				ren.hFlip = frame.hFlip;
				offset.offset = frame.offset;


				if (anim.shadowClip != nullptr) {
					const SpriteFrame& shadowFrame = anim.shadowClip->GetFrame(anim.clipFrame);
					ren.shadowSprite = shadowFrame.sprite.image;
					offset.shadowOffset = shadowFrame.offset;
				}
				else {
					//rcomp.shadowSprite.rect.size = { 0,0 };
				}
				if (anim.unitColorClip != nullptr) {
					ren.colorSprite = anim.unitColorClip->GetFrame(anim.clipFrame).sprite.image;
				}
				else {
					ren.unitColor = 0;
				}

				archetype.changed[i]->changed = true;
			}
		}
	}
}

void AnimationSystem::UpdateAnimations(AnimationArchetype& archetype) {

	a = &archetype;

	JobSystem::RunJob(archetype.size(), JobSystem::DefaultJobSize, UpdateAnims);

}
