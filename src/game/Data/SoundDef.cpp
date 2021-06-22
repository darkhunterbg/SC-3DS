#include "SoundDef.h"
#include "../StringLib.h"
#include "../Engine/AssetLoader.h"


void AdvisorSounds::LoadSoundClips()
{
	char buffer[128];
	std::string pattern = SoundPath;
	pattern += "%02i";

	for (uint8_t id : SoundIds) {
		stbsp_snprintf(buffer, sizeof(buffer), pattern.data(), id);
		Clips[id] = AssetLoader::LoadAudioClip(buffer);
	}
}

void AudioStreamDef::Load()
{
	Stream = AssetLoader::LoadAudioClip(Path);
}
