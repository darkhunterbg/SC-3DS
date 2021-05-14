#pragma once

#include "AnimationDef.h"
#include "Assets.h"
#include <functional>

struct UnitGraphicsDef {
	Vector2Int16 RenderSize;
	Rectangle16 Collider;

	UnitDirectionalAnimationDef MovementAnimationDef;
	UnitDirectionalAnimationDef IdleAnimationDef;
	UnitDirectionalAnimationDef AttackAnimationDef;
	UnitAnimationDef DeathAnimationDef;

	UnitAnimationClip MovementAnimations[32];
	UnitAnimationClip AttackAnimations[32];
	UnitAnimationClip IdleAnimations [32];
	UnitAnimationClip DeathAnimation;

	std::function<void()>  LoadResourcesAction;

	UnitGraphicsDef() {}
	UnitGraphicsDef(const UnitGraphicsDef&) = delete;
	UnitGraphicsDef& operator=(const UnitGraphicsDef&) = delete;
};