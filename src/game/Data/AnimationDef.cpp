#include "AnimationDef.h"
#include "../Assets.h"
#include "../Debug.h"

void AnimationDef::GenerateAnimation(const SpriteFrameAtlas* a, AnimationClip& clip) {

	clip.looping = Looping;
	clip.frameTime = FrameTime;

	for (int i = 0; i < FrameCount; ++i)
		clip.AddFrameCentered(a->GetFrame(FrameStart + i), Vector2Int16(a->FrameSize));
}

void DirectionalAnimationDef::GenerateAnimations(const SpriteFrameAtlas* a, AnimationClip clips[32]) {
	for (int i = 0; i <= 16; ++i) {
		clips[i].looping = Looping;
		clips[i].frameTime = FrameTime;

		for (int j = 0; j < FrameCount; ++j)
			clips[i].AddFrameCentered(a->GetFrame(FrameStart + i + j * 17), Vector2Int16(a->FrameSize));
	}

	for (int i = 17; i < 32; ++i) {
		clips[i].looping = Looping;
		clips[i].frameTime = FrameTime;

		for (int j = 0; j < FrameCount; ++j) 
			clips[i].AddFrameCentered(a->GetFrame(FrameStart + 32 - i + j * 17), Vector2Int16(a->FrameSize), true);
	}
}

void UnitAnimationDef::GenerateAnimation(const SpriteFrameAtlas* a, const SpriteFrameAtlas* sa, UnitAnimationClip& clip)
{
	clip.looping = Looping;
	clip.frameTime = FrameTime;

	for (int i = 0; i < FrameCount; ++i) {
		clip.AddFrameCentered(a->GetFrame(FrameStart + i), Vector2Int16(a->FrameSize));
		if (sa != nullptr)
			clip.AddShadowFrameCentered(i, sa->GetFrame(FrameStart + i), Vector2Int16(sa->FrameSize), ShadowAdditionalOffset);
		if (UnitColorFrameStart >= 0)
			clip.AddColorFrame(i, a->GetFrame(UnitColorFrameStart + i));
	}
}

void UnitDirectionalAnimationDef::GenerateAnimations(const SpriteFrameAtlas* a, const SpriteFrameAtlas* sa, UnitAnimationClip clips[32])
{
	for (int i = 0; i <= 16; ++i) {
		clips[i].looping = Looping;
		clips[i].frameTime = FrameTime;

		for (int j = 0; j < FrameCount; ++j) {

			clips[i].AddFrameCentered(a->GetFrame(FrameStart + i + j * 17), Vector2Int16(a->FrameSize));
			if (sa != nullptr)
				clips[i].AddShadowFrameCentered(j, sa->GetFrame(FrameStart + i + j * 17), Vector2Int16(sa->FrameSize), ShadowOffset);

			if (UnitColorFrameStart >= 0)
				clips[i].AddColorFrame(j, a->GetFrame(UnitColorFrameStart + i + j * 17));

		}
	}

	for (int i = 17; i < 32; ++i) {
		clips[i].looping = Looping;
		clips[i].frameTime = FrameTime;

		for (int j = 0; j < FrameCount; ++j) {
			clips[i].AddFrameCentered(a->GetFrame(FrameStart + 32 - i + j * 17), Vector2Int16(a->FrameSize) , true);
			if (sa != nullptr)
				clips[i].AddShadowFrameCentered(j, sa->GetFrame(FrameStart + 32 - i + j * 17), Vector2Int16(sa->FrameSize), ShadowOffset);

			if (UnitColorFrameStart >= 0)
				clips[i].AddColorFrame(j, a->GetFrame(UnitColorFrameStart + 32 - i + j * 17));
		}
	}


}

