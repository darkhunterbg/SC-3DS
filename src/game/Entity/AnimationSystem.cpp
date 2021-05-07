#include "AnimationSystem.h"
#include "RenderSystem.h"
#include <cstring>



void AnimationSystem::UpdateAnimations(RenderSystem& renderSystem) {
	/*
	int cid = 0;
	for (AnimationComponent& cmp : AnimationComponents.GetComponents()) {
		if (!cmp.pause)
		{

			cmp.frameCountdown--;
			if (cmp.frameCountdown == 0) {
				cmp.clipFrame++;

				cmp.frameCountdown = cmp.clip->frameDuration;
				int framesCount = cmp.clip->GetFrameCount();
				if (cmp.clip->looping)
					cmp.clipFrame %= framesCount;

				if (cmp.clipFrame < framesCount)
				{
					EntityId eid = AnimationComponents.GetEntityIdForComponent(cid);
					RenderComponent& rcomp = renderSystem.RenderComponents.GetComponent(eid);
					const SpriteFrame& frame = cmp.clip->GetFrame(cmp.clipFrame);
					rcomp.SetFrame(frame);

					if (cmp.shadowClip != nullptr) {
						const SpriteFrame& shadowFrame = cmp.shadowClip->GetFrame(cmp.clipFrame);
						rcomp.SetShadowFrame(shadowFrame);
					}
					else {
						//rcomp.shadowSprite.rect.size = { 0,0 };
					}
					if (cmp.unitColorClip != nullptr) {
						rcomp.colorSprite = cmp.unitColorClip->GetFrame(cmp.clipFrame).sprite.image;
					}
					else {
						rcomp.unitColor = 0;
					}
				}
			}
		}
		++cid;
	}
	*/
}
