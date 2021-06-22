#include "AssetLoader.h"
#include "Platform.h"
#include "Debug.h"
#include "../Loader/Wave.h"
#include "../Loader/BinaryData.h"

static constexpr const int AudioStreamBufferSize = 4096;
static AudioClip LoadAudioClipFromFile(const char* path);
static AudioStream* LoadAudioStreamFromFile(const char* path);

static constexpr const char* DataFile = "data.bin";

static std::hash<int> intHasher;

AssetLoader AssetLoader::instance;

const Texture* AssetLoader::LoadTexture(const char* path)
{
	std::string p = path;
	AssetId id = instance.hasher(p);
	AssetEntry& e = instance.loadedAssets[id];

	if (e.id == 0) {
		Vector2Int16 size;
		TextureId texId = Platform::LoadTexture(p.data(),size);
		e.data = new Texture(p, size, texId);
		e.type = AssetType::Texture;
		e.id = id;
	}
	else {
		if (e.type != AssetType::Texture)
			EXCEPTION("Tried to load '%s' as %i but it was %i!",
				p.data(), GetAssetTypeName(AssetType::Texture), GetAssetTypeName(e.type));
	}

	return (const Texture*)e.data;
}

const Font* AssetLoader::LoadFont(const char* path, int size)
{
	std::string p = path;
	AssetId id = instance.hasher(p);
	id = (id * 513269) ^ intHasher(size);
	AssetEntry& e = instance.loadedAssets[id];

	if (e.id == 0) {
		e.data = Platform::LoadFont(p.data(), size);
		e.type = AssetType::Font;
		e.id = id;
	}
	else {
		if (e.type != AssetType::Font)
			EXCEPTION("Tried to load '%s' as %i but it was %i!",
				p.data(), GetAssetTypeName(AssetType::Font), GetAssetTypeName(e.type));
	}

	return (const Font*)e.data;
}

AudioClip AssetLoader::LoadAudioClip(const char* path)
{
	std::string p = path;
	AssetId id = instance.hasher(p);
	AssetEntry& e = instance.loadedAssets[id];

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

	AssetId id = instance.hasher(p);
	AssetEntry& e = instance.loadedAssets[id];

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
	std::string p = path;
	p += ".wav";
	FILE* f = Platform::OpenAsset(p.data());

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

void AssetLoader::LoadDatabase()
{
	FILE* f = Platform::OpenAsset(DataFile);

	if (!f)
		EXCEPTION("Failed to load file %s", DataFile);

	std::string p = DataFile;
	AssetId id = instance.hasher(p);
	AssetEntry& e = instance.loadedAssets[id];

	instance.db = BinaryDataLoader::LoadDatabase(f);

	e.id = id;
	e.type = AssetType::Database;
	e.data = instance.db;

	instance.db->LoadAssetReferences();

	fclose(f);
}
