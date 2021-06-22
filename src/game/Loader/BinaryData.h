#pragma once

#include <cstdint>
#include <vector>
#include <cstdio>


class GameDatabase;

enum class DataSectionType : uint32_t
{
	None = 0,
	Atlases = 1,
	Images = 2,
	Frames = 3,
	Sprites = 4,
	AnimClips = 5,
	AnimInstructions = 6,
	Units = 7,
	Abilities = 8,
	Upgrades = 9,
	Effects = 10,
	Wireframe = 11,
	Weapons = 12,
	SoundSets = 13,
	AudioClips = 14,
};


struct DataSectionHeader {
	DataSectionType type;
	uint32_t size;
	uint32_t offset;
};

class BinaryDataLoader {
private:
	BinaryDataLoader() = delete;
	~BinaryDataLoader() = delete;
public:
	static GameDatabase* LoadDatabase(FILE* f);
};