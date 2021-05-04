#pragma once

#include <string>

#include "../Assets.h"
#include "../MathLib.h"

struct UnitDef {

	std::string Name;
	unsigned Health;
	unsigned MovementSpeed;
	unsigned RotationSpeed;

	Rectangle Collider;

	AnimationClip MovementAnimations[32];
	AnimationClip MovementAnimationsShadow[32];
	AnimationClip MovementAnimationsTeamColor[32];

	AnimationClip AttackAnimations[32];
	AnimationClip DeathAnimation;
};