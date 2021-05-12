#pragma once

#include <string>

#include "../Assets.h"
#include "../MathLib.h"
#include "AnimationDef.h"
#include "SoundDef.h"

struct UnitDef {

	std::string Name;
	unsigned Health;
	uint8_t MovementSpeed;
	uint8_t RotationSpeed;

	Vector2Int16 RenderSize;
	Rectangle16 Collider;

	SoundDef SelectedSoundDef;
	SoundDef ActionConfirmSoundDef;
	SoundDef DeathSoundDef;

	UnitDirectionalAnimationDef MovementAnimationDef;
	UnitAnimationDef DeathAnimationDef;
	

	UnitAnimationClip MovementAnimations[32];
	UnitAnimationClip AttackAnimations[32];
	UnitAnimationClip DeathAnimation;



	UnitDef() {}
	UnitDef(const UnitDef&) = delete;
	UnitDef& operator=(const UnitDef&) = delete;
};