#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "GraphicsDef.h"
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
	SpriteAtlasDef ConsoleSprite;

	const SpriteFrameAtlas* CommandIconsAtlas;

	Sprite SupplyIcon;
	uint8_t SupplyIconId;

	Sprite GasIcon;
	uint8_t GasIconId;

	AdvisorSounds AdvisorErrorSounds;
	AdvisorSounds AdvisorUpdateSounds;

	std::vector<AudioStreamDef> GameMusic;

	std::function<void()> CommandIconsLoad;

	RaceDef() {  }
	RaceDef(const RaceDef&) = delete;
	RaceDef& operator=(const RaceDef&) = delete;

	void LoadResourses();
};

