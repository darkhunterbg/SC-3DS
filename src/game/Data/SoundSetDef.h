#pragma once

#include <cstdint>
#include <Span.h>

class AudioClip;

#pragma pack(push, 2)
struct SoundSetDef {
	char Path[32];
	uint16_t ClipStart;
	uint8_t ClipCount;
	bool Randomize;

	AudioClip& GetAudioClip(int index) const;
	Span<AudioClip*> GetAudioClips() const;
};
#pragma pack(pop)