#include "SoundDef.h"
#include "../StringLib.h"
#include "../Engine/AssetLoader.h"

void UnitSound::LoadSoundClips()
{
	char buffer[128];
	std::string pattern = SoundPath;

	if (DirectFile)
		pattern += ".wav";
	else
		pattern += "%02i.wav";
	for (int i = 0; i < TotalClips; ++i) {
		stbsp_snprintf(buffer, sizeof(buffer), pattern.data(), i);
		Clips[i] = AssetLoader::LoadAudioClip(buffer);
	}
}

void AdvisorSounds::LoadSoundClips()
{
	char buffer[128];
	std::string pattern = SoundPath;
	pattern += "%02i.wav";

	for (uint8_t id : SoundIds) {
		stbsp_snprintf(buffer, sizeof(buffer), pattern.data(), id);
		Clips[id] = AssetLoader::LoadAudioClip(buffer);
	}
}

void AudioStreamDef::Load()
{
	Stream = AssetLoader::LoadAudioStream(Path);
}
