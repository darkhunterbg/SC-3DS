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

	int animOffset = 0;

	for (int i = 0; i <= 16; ++i) {
		clips[i].looping = Looping;
		clips[i].frameTime = FrameTime;

		for (int j = 0; j < FrameCount; ++j)
			clips[i].AddFrameCentered(a->GetFrame(FrameStart + animOffset + j * 17), Vector2Int16(a->FrameSize));

		animOffset += MultiDirectional;
	}

	if (MultiDirectional)
		MultiDirectional = 32;

	for (int i = 17; i < 32; ++i) {
		clips[i].looping = Looping;
		clips[i].frameTime = FrameTime;

		for (int j = 0; j < FrameCount; ++j) 
			clips[i].AddFrameCentered(a->GetFrame(FrameStart +  animOffset + j * 17), Vector2Int16(a->FrameSize), true);
	
		animOffset += MultiDirectional;
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
	int animOffset = 0;

	for (int i = 0; i <= 16; ++i) {
		clips[i].looping = Looping;
		clips[i].frameTime = FrameTime;

		for (int j = 0; j < FrameCount; ++j) {

			auto f = clips[i].AddFrameCentered(a->GetFrame(FrameStart + animOffset + j * 17), Vector2Int16(a->FrameSize));
			if (sa != nullptr)
				clips[i].AddShadowFrameCentered(f, sa->GetFrame(FrameStart + animOffset + j * 17), Vector2Int16(sa->FrameSize), ShadowOffset);

			if (UnitColorFrameStart >= 0)
				clips[i].AddColorFrame(f, a->GetFrame(UnitColorFrameStart + animOffset + j * 17));

		}

		animOffset += MultiDirectional;
	}

	if (MultiDirectional)
		MultiDirectional = 32;

	for (int i = 17; i < 32; ++i) {
		clips[i].looping = Looping;
		clips[i].frameTime = FrameTime;

		for (int j = 0; j < FrameCount; ++j) {
			auto f = clips[i].AddFrameCentered(a->GetFrame(FrameStart  + animOffset + j * 17), Vector2Int16(a->FrameSize) , true);
			if (sa != nullptr)
				clips[i].AddShadowFrameCentered(f, sa->GetFrame(FrameStart + animOffset + j * 17), Vector2Int16(sa->FrameSize), ShadowOffset);

			if (UnitColorFrameStart >= 0)
				clips[i].AddColorFrame(f, a->GetFrame(UnitColorFrameStart + animOffset + j * 17));
		}

		animOffset -= MultiDirectional;
	}


}

