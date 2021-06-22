#pragma once

#include "../Assets.h"
#include <vector>

struct AdvisorSounds {
	const char* SoundPath;

	std::vector<uint8_t> SoundIds;
	std::array< AudioClip*, 8> Clips;

	void LoadSoundClips();
};

struct AudioStreamDef {
	const char* Path;
	AudioClip* Stream;

	void Load();
};