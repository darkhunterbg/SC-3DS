#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <functional>

#include "SoundDef.h"

enum class RaceType :uint8_t {

	Neutral = 0,
	Terran = 1,
	Zerg = 2,
	Protoss = 3
};

struct RaceDef {
	RaceType Type;
	std::string Name;

	std::string consolePath;
	std::string commandIconsPath;

	ImageFrame ConsoleLowerSprite;
	ImageFrame ConsoleUpperSprite;
	const Image* CommandIcons;

	ImageFrame SupplyIcon;
	std::string supplyIconPath;

	std::string SupplyNamePlural;
	std::string SupplyBuildingNamePlural;

	ImageFrame GasIcon;
	std::string gasIconPath;

	AdvisorSounds AdvisorErrorSounds;
	AdvisorSounds AdvisorUpdateSounds;

	std::vector<AudioStreamDef> GameMusic;

	RaceDef() {  }
	RaceDef(const RaceDef&) = delete;
	RaceDef& operator=(const RaceDef&) = delete;

	void LoadResourses();
};

