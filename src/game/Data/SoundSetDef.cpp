#include "SoundSetDef.h"
#include "GameDatabase.h"

const AudioClip& SoundSetDef::GetAudioClip(int index) const
{
	return GameDatabase::instance->GetAudioClip(ClipStart + index);
}

Span<AudioClip> SoundSetDef::GetAudioClips() const
{
	const AudioClip* start = &GameDatabase::instance->GetAudioClip(ClipStart);
	return { start, ClipCount };
}
