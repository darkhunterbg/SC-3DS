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
	unsigned Health = 100;
	uint8_t MovementSpeed = 1;
	uint8_t Acceleration = 0;
	uint8_t RotationSpeed = 1;
	uint8_t Vision = 1;
	
	bool IsBuilding = false;
	bool IsResourceContainer = false;

	uint8_t UseSupplyDoubled = 0;
	uint8_t ProvideSupplyDoubled = 0;

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

	inline void SetUseSupply(uint8_t amount, bool half = false) {
		UseSupplyDoubled = (amount << 1)+ half;
	}
	inline void SetPovideSupply(uint8_t amount, bool half = false) {
		ProvideSupplyDoubled = (amount << 1) + half;
	}


	inline void LoadAllSounds() {

		UnitSound* s = (UnitSound*)&Sounds;
		for (int i = 0; i < 5; ++i) {
			if (s[i].TotalClips > 0)
				s[i].LoadSoundClips();
		}
	}
};