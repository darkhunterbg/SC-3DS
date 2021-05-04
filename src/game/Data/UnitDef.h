#pragma once

#include <string>

#include "../Assets.h"
#include "../MathLib.h"
#include "AnimationDef.h"

struct UnitDef {

	std::string Name;
	unsigned Health;
	unsigned MovementSpeed;
	unsigned RotationSpeed;

	Rectangle Collider;

	DirectionalAnimationDef MovementAnimationDef;
	AnimationDef DeathAnimationDef;

	AnimationClip MovementAnimations[32];
	AnimationClip MovementAnimationsShadow[32];
	AnimationClip MovementAnimationsTeamColor[32];

	AnimationClip AttackAnimations[32];
	AnimationClip DeathAnimation;
};