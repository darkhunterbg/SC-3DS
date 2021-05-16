#include "SoundDef.h"
#include "../StringLib.h"
#include "../Game.h"

void UnitSound::LoadSoundClips()
{
	char buffer[128];
	std::string pattern = SoundPath;
	pattern += "%02i.wav";
	for (int i = 0; i < TotalClips; ++i) {
		stbsp_snprintf(buffer, sizeof(buffer), pattern.data(), i);
		Clips[i] = Game::AssetLoader.LoadAudioClip(buffer);
	}
}

void AdvisorSounds::LoadSoundClips()
{
	char buffer[128];
	std::string pattern = SoundPath;
	pattern += "%02i.wav";

	for (uint8_t id : SoundIds) {
		stbsp_snprintf(buffer, sizeof(buffer), pattern.data(), id);
		Clips[id] = Game::AssetLoader.LoadAudioClip(buffer);
	}
}

void AudioStreamDef::Load()
{
	Stream = Game::AssetLoader.LoadAudioStream(Path);
}
