#pragma once

#include "MathLib.h"
#include <string>
#include <vector>
#include <cstdio>


enum class DataSectionType : uint32_t
{
	None = 0,
	Atlases = 1,
	Images = 2,
	Frames = 3,
	Sprites = 4,
	AnimClips = 5,
	AnimInstructions = 6,
};


struct DataSectionHeader {
	DataSectionType type;
	uint32_t size;
	uint32_t offset;
};

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

#pragma pack(1)
struct ImageFrameDef {
	Vector2Int16 offset;
	Vector2Int16 size;
	Vector2Int16 atlasOffset;
	uint8_t atlasId;
};

#pragma pack(1)
struct SpriteDef {
	char name[32];
	char image[32];
	bool isRotating;
};

#pragma pack(1)
struct AnimClipDef {
	char sprite[32];
	uint32_t instructionStart;
	uint8_t instructionCount;
	uint8_t type;
};

#pragma pack(1)
struct AnimInstructionDef {
	uint8_t id;
	uint8_t params[4];
};

class GameDatabase {

public:
	GameDatabase(FILE* f);

	std::vector<AtlasDef> Atlases;
	std::vector<ImageDef> Images;
	std::vector<ImageFrameDef> Frames;
	std::vector<SpriteDef> Sprites;
	std::vector<AnimClipDef> AnimationClips;
	std::vector<AnimInstructionDef> AnimationInstructions;
};