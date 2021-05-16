#pragma once

#include <string>
#include <functional>

#include "../Assets.h"
#include "../MathLib.h"
#include "AnimationDef.h"
#include "SoundDef.h"
#include "GraphicsDef.h"

struct UnitDef {
	std::string Name;
	unsigned Health;
	uint8_t MovementSpeed;
	uint8_t Acceleration = 0;
	uint8_t RotationSpeed;

	struct {
		UnitSound Ready;
		UnitSound Yes;
		UnitSound What;
		UnitSound Annoyed;
		UnitSound Death;
	} Sounds;

	const UnitGraphicsDef* Graphics = nullptr;

	UnitDef() {  }
	UnitDef(const UnitDef&) = delete;
	UnitDef& operator=(const UnitDef&) = delete;

	inline void LoadAllSounds() {

		UnitSound* s = (UnitSound*)&Sounds;
		for (int i = 0; i < 5; ++i) {
			if (s[i].TotalClips > 0)
				s[i].LoadSoundClips();
		}
	}
};