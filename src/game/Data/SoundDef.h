#pragma once

#include "../Assets.h"

struct SoundDef {

	const char* SoundPath;
	uint8_t TotalClips = 0;

	std::array<AudioClip, 8> Clips;

	void LoadSoundClips();
};