#pragma once

class SpriteFrameAtlas;
class AnimationClip;

struct DirectionalAnimationDef {
	int FrameStart;
	int FrameDuration;
	int UnitColorFrameStart;
	bool Looping;

	void GenerateAnimations(const SpriteFrameAtlas* a, const SpriteFrameAtlas* sa,
		AnimationClip* main, AnimationClip* shadow, AnimationClip* color);
};

struct AnimationDef {
	int FrameStart;
	int FrameDuration;
	bool Looping;

	void GenerateAnimation(const SpriteFrameAtlas* a, AnimationClip* clip);
};