#include "SoundSetDef.h"
#include "GameDatabase.h"

AudioClip& SoundSetDef::GetAudioClip(int index) const
{
	return GameDatabase::instance->GetAudioClip(ClipStart + index);
}

Span<AudioClip*> SoundSetDef::GetAudioClips() const
{
	return GameDatabase::instance->GetAudioClips(ClipStart, ClipCount);
}
