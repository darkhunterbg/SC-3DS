#pragma once
#include <cstdint>

#include <Span.h>

class VideoClip;

#pragma pack(push, 1)
struct UnitPortraitDef {
	char Path[32];
	uint16_t IdleClipStart;
	uint8_t IdleClipCount;
	uint16_t ActivatedClipStart;
	uint8_t ActivatedClipCount;

	VideoClip& GetIdleClip(int index) const;
	Span<VideoClip*> GetIdleClips() const;

	VideoClip& GetActivatedClip(int index) const;
	Span<VideoClip*> GetActivatedClips() const;
};
#pragma pack(pop)