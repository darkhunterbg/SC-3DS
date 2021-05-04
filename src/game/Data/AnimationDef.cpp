#include "AnimationDef.h"
#include "../Assets.h"

void DirectionalAnimationDef::GenerateAnimations(const SpriteFrameAtlas* a, const SpriteFrameAtlas* as,
	AnimationClip* main, AnimationClip* shadow, AnimationClip* color) {


	for (int i = 0; i <= 16; ++i) {
		main[i].looping = Looping;
		main[i].frameDuration = 1;

		for (int j = 0; j < FrameDuration; ++j) {
			main[i].AddFrameCentered(a->GetFrame(i + j * 17 + FrameStart), a->FrameSize);
			color[i].AddFrameCentered(a->GetFrame(i + j * 17 + UnitColorFrameStart), a->FrameSize);
		}

		shadow[i].looping = Looping;
		shadow[i].frameDuration = 1;
		for (int j = 0; j < FrameDuration; ++j)
			shadow[i].AddFrameCentered(as->GetFrame(i + j * 17 + FrameStart), as->FrameSize);

	}

	for (int i = 17; i < 32; ++i) {
		main[i].looping = Looping;
		main[i].frameDuration = 1;

		for (int j = 0; j < FrameDuration; ++j) {
			main[i].AddFrameCentered(a->GetFrame(32 - i + j * 17 + FrameStart), a->FrameSize, true);
			color[i].AddFrameCentered(a->GetFrame(32 - i + j * 17 + UnitColorFrameStart), a->FrameSize);
		}

		shadow[i].looping = Looping;
		shadow[i].frameDuration = 1;
		for (int j = 0; j < FrameDuration; ++j)
			shadow[i].AddFrameCentered(as->GetFrame(32 - i + j * 17 + FrameStart), as->FrameSize, true);

	}
}

void AnimationDef::GenerateAnimation(const SpriteFrameAtlas* a, AnimationClip* clip) {
	for (int i = 0; i < FrameDuration; ++i)
		clip->AddFrameCentered(a->GetFrame(FrameStart + i), a->FrameSize);
}