#pragma once

#include "../MathLib.h"
#include "SoundSetDef.h"

struct ImageFrame;

#pragma pack(push,2)
struct WeaponDef {
	
	uint16_t IconId;
	char Name[32];
	uint16_t SpawnSound;

	const ImageFrame& GetIcon() const;
	const SoundSetDef* GetSpawnSound() const;
};
#pragma pack(pop)