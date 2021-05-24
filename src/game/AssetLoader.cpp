#include "AssetLoader.h"
#include "Platform.h"
#include "Debug.h"
#include "Loader/Wave.h"

static constexpr const int AudioStreamBufferSize = 4096;
static AudioClip LoadAudioClipFromFile(const char* path);
static AudioStream* LoadAudioStreamFromFile(const char* path);

const SpriteAtlas* AssetLoader::LoadAtlas(const char* path)
{
	std::string p = path;
	AssetId id = hasher(p);
	AssetEntry& e = loadedAssets[id];

	if (e.id == 0) {
		e.data = Platform::LoadAtlas(p.data());
		e.type = AssetType::SpriteAtlas;
		e.id = id;
	}
	else {
		if (e.type != AssetType::SpriteAtlas)
			EXCEPTION("Tried to load '%s' as %i but it was %i!",
				p.data(), GetAssetTypeName(AssetType::SpriteAtlas), GetAssetTypeName(e.type));
	}

	return  (const SpriteAtlas*)e.data;
}

Font AssetLoader::LoadFont(const char* path)
{
	std::string p = path;
	AssetId id = hasher(p);
	AssetEntry& e = loadedAssets[id];

	if (e.id == 0) {
		e.data = new Font(Platform::LoadFont(p.data()));
		e.type = AssetType::Font;
		e.id = id;
	}
	else {
		if (e.type != AssetType::Font)
			EXCEPTION("Tried to load '%s' as %i but it was %i!",
				p.data(), GetAssetTypeName(AssetType::Font), GetAssetTypeName(e.type));
	}

	return  *(const Font*)e.data;
}


AudioClip AssetLoader::LoadAudioClip(const char* path)
{
	std::string p = path;
	AssetId id = hasher(p);
	AssetEntry& e = loadedAssets[id];

	if (e.id == 0) {
		e.data = new AudioClip(LoadAudioClipFromFile(p.data()));
		e.type = AssetType::AudioClip;
		e.id = id;
	}
	else {
		if (e.type != AssetType::AudioClip)
			EXCEPTION("Tried to load '%s' as %i but it was %i!",
				p.data(), GetAssetTypeName(AssetType::AudioClip), GetAssetTypeName(e.type));
	}

	return  *(const AudioClip*)e.data;
}


AudioStream* AssetLoader::LoadAudioStream(const char* path)
{
	std::string p = path;

	AssetId id = hasher(p);
	AssetEntry& e = loadedAssets[id];

	if (e.id == 0) {
		e.data = LoadAudioStreamFromFile(p.data());
		e.type = AssetType::AudioStream;
		e.id = id;
	}
	else {
		if (e.type != AssetType::AudioStream)
			EXCEPTION("Tried to load '%s' as %i but it was %i!",
				p.data(), GetAssetTypeName(AssetType::AudioStream), GetAssetTypeName(e.type));
	}

	return  (AudioStream*)e.data;
}


static uint16_t audioClipId = 0;

static AudioClip LoadAudioClipFromFile(const char* path) {
	FILE* f = Platform::OpenAsset(path);

	if (f == nullptr)
		EXCEPTION("Failed to open asset '%s'!", path);

	AudioInfo info;

	if (!WaveLoader::ReadWAVHeader(f, &info))
		EXCEPTION("Failed to read WAV header in '%s'!", path);

	uint8_t* data = new uint8_t[info.GetTotalSize()];

	if (!WaveLoader::LoadWAVData(f, { data, (unsigned)info.GetTotalSize() }, info))
		EXCEPTION("Failed to load WAV data in '%s'!", path);

	AudioClip clip = { info, data, ++audioClipId };

	fclose(f);

	return clip;

}
static AudioStream* LoadAudioStreamFromFile(const char* path) {
	FILE* f = Platform::OpenAsset(path);

	if (f == nullptr)
		EXCEPTION("Failed to open asset '%s'!", path);

	AudioInfo info;

	if (!WaveLoader::ReadWAVHeader(f, &info))
		EXCEPTION("Failed to read WAV header in '%s'!", path);

	AudioStream* stream = new AudioStream(info, AudioStreamBufferSize, f);

	return stream;
}
