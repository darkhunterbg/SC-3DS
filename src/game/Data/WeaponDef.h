#pragma once

#include "../MathLib.h"
#include "SoundDef.h"
#include <string>

struct WeaponDef {
	std::string Name;

	uint16_t Damage = 1;
	uint8_t Cooldown = 1;

	UnitSound Sound;

	WeaponDef() {}
	WeaponDef(const WeaponDef&) = delete;
	WeaponDef& operator=(const WeaponDef&) = delete;

	inline void LoadSound() {
		if (Sound.TotalClips > 0)
			Sound.LoadSoundClips();
	}
};