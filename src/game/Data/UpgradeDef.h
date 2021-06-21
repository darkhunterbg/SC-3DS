#pragma once

#include <cstdint>

struct ImageFrame;

typedef uint32_t UpgradeId;

#pragma pack(push,2)
struct UpgradeDef {
	UpgradeId Id;
	uint16_t IconId;
	char Name[32];

	const ImageFrame& GetIcon() const;

};
#pragma pack(pop)