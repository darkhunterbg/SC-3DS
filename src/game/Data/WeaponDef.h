#pragma once

#include "../MathLib.h"
#include "SoundDef.h"
#include "AnimationDef.h"
#include <string>
#include <functional>

struct WeaponDef {
	std::string Name;

	uint16_t Damage = 1;
	uint8_t MaxRange = 1;
	uint8_t Cooldown = 1;

	UnitSound Sound;
	DirectionalAnimationDef TargetEffectDef;
	AnimationClip TargetEffect[32];

	WeaponDef() {}
	WeaponDef(const WeaponDef&) = delete;
	WeaponDef& operator=(const WeaponDef&) = delete;

	std::function<void()> LoadGraphicsAction;

	inline void LoadSound() {
		if (Sound.TotalClips > 0)
			Sound.LoadSoundClips();
	}
};