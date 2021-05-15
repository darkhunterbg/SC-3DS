#pragma once

#include "AnimationDef.h"
#include "Assets.h"
#include "../MathLib.h"
#include <functional>

struct UnitGraphicsDef {
	Vector2Int16 RenderSize;
	Rectangle16 Collider;

	UnitDirectionalAnimationDef MovementAnimationDef;
	UnitDirectionalAnimationDef IdleAnimationDef;
	UnitDirectionalAnimationDef AttackAnimationDef;
	UnitAnimationDef DeathAnimationDef;

	DirectionalAnimationDef MovementGlowAnimationDef;

	UnitAnimationClip MovementAnimations[32];
	AnimationClip MovementGlowAnimations[32];
	UnitAnimationClip AttackAnimations[32];
	UnitAnimationClip IdleAnimations [32];
	UnitAnimationClip DeathAnimation;

	

	struct {
		AnimationDef Def;
		AnimationClip Clip;
		int Depth = 0;

	} Remnants;

	std::function<void()>  LoadResourcesAction;

	inline bool HasMovementGlow() const { return MovementGlowAnimationDef.FrameCount > 0; }
	inline bool HasRemnants() const { return Remnants.Def.FrameCount > 0; };

	UnitGraphicsDef() {}
	UnitGraphicsDef(const UnitGraphicsDef&) = delete;
	UnitGraphicsDef& operator=(const UnitGraphicsDef&) = delete;
};