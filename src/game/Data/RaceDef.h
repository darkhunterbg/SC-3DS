#pragma once

#include <cstdint>
#include <string>

#include "../Assets.h"

struct SoundSetDef;

enum AdvisorErrorMessageType {
	NotEnoughMinerals = 0,
	NotEnoughGas = 1,
	NotEnoughSupply = 2,
	LandingInterrupted = 3,
	InvalidLandingZone = 4,
	NotEnoughEnergy = 6,
};


enum AdvisorUpdateMessageType {

};


enum class RaceType :uint8_t {

	Neutral = 0,
	Terran = 1,
	Zerg = 2,
	Protoss = 3
};

struct RaceDef {
	RaceType Type;
	std::string Name;

	ImageFrame ConsoleSprite;
	ImageFrame ConsoleLowerSprite;
	ImageFrame ConsoleUpperSprite;
	const Image* CommandIcons;

	ImageFrame SupplyIcon;
	ImageFrame GasIcon;

	std::string SupplyNamePlural;
	std::string SupplyBuildingNamePlural;

	const SoundSetDef* GetAdvisorError(AdvisorErrorMessageType type) const;
	const SoundSetDef* GetAdvisorUpdate(AdvisorUpdateMessageType type) const;
	const SoundSetDef* GetMusic() const;
};

