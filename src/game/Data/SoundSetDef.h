#pragma once

#include <cstdint>
#include <Span.h>

struct AudioClip;

#pragma pack(push, 2)
struct SoundSetDef {
	uint16_t ClipStart;
	uint8_t ClipCount;
	bool Randomize;

	const AudioClip& GetAudioClip(int index) const;
	Span<AudioClip> GetAudioClips() const;
};
#pragma pack(pop)