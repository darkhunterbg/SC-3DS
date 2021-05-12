#include "SoundDef.h"
#include "../StringLib.h"
#include "../Platform.h"

void SoundDef::LoadSoundClips()
{
	char buffer[128];
	std::string pattern = SoundPath;
	pattern += "%02i.wav";
	for (int i = 0; i < TotalClips; ++i) {
		stbsp_snprintf(buffer, sizeof(buffer), pattern.data(), i);
		Clips[i] = Platform::LoadAudioClip(buffer);
	}
}
