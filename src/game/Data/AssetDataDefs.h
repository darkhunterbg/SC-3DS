#pragma once

#include <string>
#include "../MathLib.h"

#pragma pack(push,1)
struct AtlasDef {
	char name[32];
	uint16_t index;

	std::string GetAtlasName() const {
		return std::string(name) + "_" + std::to_string(index);
	}
};
struct ImageDef {
	char name[32];
	uint16_t frameStart;
	uint16_t frameCount;
	Vector2Int16 size;
};


struct ImageFrameDef {
	Vector2Int16 offset;
	Vector2Int16 size;
	Vector2Int16 atlasOffset;
	uint8_t atlasId;
};

struct SpriteDef {
	char name[32];
	uint16_t imageId;
	bool isRotating;
};

struct AnimClipDef {
	uint16_t spriteId;
	uint32_t instructionStart;
	uint8_t instructionCount;
	uint8_t type;
};

struct AnimInstructionDef {
	uint8_t id;
	uint8_t params[4];
};

#pragma pack(pop)