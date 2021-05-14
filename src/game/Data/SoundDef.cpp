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
