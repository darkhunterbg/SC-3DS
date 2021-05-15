#pragma once

#include "..\MathLib.h"

class SpriteFrameAtlas;
class UnitAnimationClip;
class AnimationClip;

struct AnimationDef {
	int FrameStart = 0;
	int FrameCount = 0;
	int FrameTime = 1;
	bool Looping = false;

	AnimationDef() {}
	AnimationDef(const AnimationDef&) = delete;
	AnimationDef& operator=(const AnimationDef&) = delete;

	void GenerateAnimation(const SpriteFrameAtlas* a, AnimationClip& clip);
};
struct DirectionalAnimationDef {
	int FrameStart = 0;
	int FrameCount = 0;
	int FrameTime = 1;
	bool Looping = false;

	DirectionalAnimationDef() {}
	DirectionalAnimationDef(const DirectionalAnimationDef&) = delete;
	DirectionalAnimationDef& operator=(const DirectionalAnimationDef&) = delete;

	void GenerateAnimations(const SpriteFrameAtlas* a, AnimationClip clips[32]);
};

struct UnitAnimationDef {
	int FrameStart = 0;
	int FrameCount = 1;
	int FrameTime = 1;
	int UnitColorFrameStart = 0;
	bool Looping = false;
	Vector2Int16 ShadowAdditionalOffset;

	UnitAnimationDef() {}
	UnitAnimationDef(const UnitAnimationDef&) = delete;
	UnitAnimationDef& operator=(const UnitAnimationDef&) = delete;

	void GenerateAnimation(const SpriteFrameAtlas* a, const SpriteFrameAtlas* sa, UnitAnimationClip& clip);
};

struct UnitDirectionalAnimationDef {
	int FrameStart = 0;
	int FrameCount = 0;
	int FrameTime = 1;
	int UnitColorFrameStart = 0;
	bool Looping = false;
	Vector2Int16 ShadowOffset;

	UnitDirectionalAnimationDef() {}
	UnitDirectionalAnimationDef(const UnitDirectionalAnimationDef&) = delete;
	UnitDirectionalAnimationDef& operator=(const UnitDirectionalAnimationDef&) = delete;

	void GenerateAnimations(const SpriteFrameAtlas* a, const SpriteFrameAtlas* sa, UnitAnimationClip clips[32]);
};