#include "AnimationDef.h"
#include "../Assets.h"

void DirectionalAnimationDef::GenerateAnimations(const SpriteFrameAtlas* a, const SpriteFrameAtlas* as,
	AnimationClip* main, AnimationClip* shadow, AnimationClip* color) {


	for (int i = 0; i <= 16; ++i) {
		main[i].looping = Looping;

		for (int j = 0; j < FrameDuration; ++j) {
			main[i].AddFrameCentered(a->GetFrame(i + j * 17 + FrameStart), Vector2Int16(a->FrameSize));
			color[i].AddFrameCentered(a->GetFrame(i + j * 17 + UnitColorFrameStart), Vector2Int16(a->FrameSize));
		}

		shadow[i].looping = Looping;
		for (int j = 0; j < FrameDuration; ++j) {
			shadow[i].AddFrameCentered(as->GetFrame(i + j * 17 + FrameStart), Vector2Int16(as->FrameSize));
			shadow[i].SetFrameOffset(j, shadow[i].GetFrame(j).offset + ShadowAdditionalOffset);
		}

	}

	for (int i = 17; i < 32; ++i) {
		main[i].looping = Looping;

		for (int j = 0; j < FrameDuration; ++j) {
			main[i].AddFrameCentered(a->GetFrame(32 - i + j * 17 + FrameStart), Vector2Int16(a->FrameSize), true);
			color[i].AddFrameCentered(a->GetFrame(32 - i + j * 17 + UnitColorFrameStart), Vector2Int16( a->FrameSize));
		}

		shadow[i].looping = Looping;
		for (int j = 0; j < FrameDuration; ++j) {
			shadow[i].AddFrameCentered(as->GetFrame(32 - i + j * 17 + FrameStart), Vector2Int16( as->FrameSize), true);
			shadow[i].SetFrameOffset(j, shadow[i].GetFrame(j).offset + ShadowAdditionalOffset);
		}


	}
}

void AnimationDef::GenerateAnimation(const SpriteFrameAtlas* a, AnimationClip* clip) {
	for (int i = 0; i < FrameDuration; ++i)
		clip->AddFrameCentered(a->GetFrame(FrameStart + i), Vector2Int16(a->FrameSize));
}