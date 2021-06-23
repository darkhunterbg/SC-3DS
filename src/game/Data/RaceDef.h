#pragma once

#include <cstdint>
#include <string>

#include "../Assets.h"

#include "../Entity/Common.h"

struct SoundSetDef;

enum class RaceType :uint8_t {

	Neutral = 0,
	Terran = 1,
	Zerg = 2,
	Protoss = 3
};

struct RaceDef {
	RaceType Type;
	std::string Name;

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

