#pragma once

#include "../Assets.h"
#include <vector>

struct UnitSound {

	const char* SoundPath;
	uint8_t TotalClips = 0;
	bool DirectFile = 0;

	std::array<AudioClip, 8> Clips;

	void LoadSoundClips();
};


struct AdvisorSounds {
	const char* SoundPath;

	std::vector<uint8_t> SoundIds;
	std::array<AudioClip, 8> Clips;

	void LoadSoundClips();
};

struct AudioStreamDef {
	const char* Path;
	AudioStream* Stream;

	void Load();
};