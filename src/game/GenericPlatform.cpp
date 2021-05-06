#include "Platform.h"
#include "Loader/Wave.h"

static constexpr const int AudioStreamBufferSize = 4096;

AudioClip Platform::LoadAudioClip(const char* path) {
	FILE* f = Platform::OpenAsset(path);

	if (f == nullptr)
		EXCEPTION("Failed to open asset '%s'!", path);
	
	AudioInfo info;

	if (!WaveLoader::ReadWAVHeader(f, &info))
		EXCEPTION("Failed to read WAV header in '%s'!", path);
	
	uint8_t* data = new uint8_t[info.GetTotalSize()];

	if(!WaveLoader::LoadWAVData(f,{data, (unsigned) info.GetTotalSize()},info))
		EXCEPTION("Failed to load WAV data in '%s'!", path);

	AudioClip clip = { info, data };

	fclose(f);

	return clip;
}

AudioStream* Platform::LoadAudioStream(const char* path) {
	FILE* f = Platform::OpenAsset(path);

	if (f == nullptr)
		EXCEPTION("Failed to open asset '%s'!", path);

	AudioInfo info;

	if (!WaveLoader::ReadWAVHeader(f, &info))
		EXCEPTION("Failed to read WAV header in '%s'!", path);

	AudioStream* stream = new AudioStream(info, AudioStreamBufferSize, f);

	return stream;
 }
