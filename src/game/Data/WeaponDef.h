#pragma once

#include "../MathLib.h"

struct ImageFrame;

#pragma pack(push,2)
struct WeaponDef {
	
	uint16_t IconId;
	char Name[32];

	const ImageFrame& GetIcon() const;
};
#pragma pack(pop)