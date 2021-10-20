#include "AssetLoader.h"
#include "Platform.h"
#include "Debug.h"
#include "../Loader/Wave.h"
#include "../Loader/BinaryData.h"

static constexpr const int AudioStreamBufferSize = 4096;
static AudioClip* LoadAudioClipFromFile(const char* path);

static constexpr const char* DataFile = "data.bin";

static std::hash<int> intHasher;

AssetLoader AssetLoader::instance;

void AssetLoader::ProcessIORequests(int threadId)
{
	while (true)
	{
		IORequest* request = nullptr;

		Platform::WaitSemaphore(instance._semaphore);

		if (instance._ioQueue.TryDequeue(&request))
		{
			request->action();
			request->completed = true;
		}
	}
}

void AssetLoader::Init()
{
	instance._semaphore = Platform::CreateSemaphore();
	instance._usesIOThread = Platform::TryStartThreads(ThreadUsageType::IO, 1, ProcessIORequests) > 0;
}

const Texture* AssetLoader::LoadTexture(const char* path)
{
	std::string p = path;
	AssetId id = instance.hasher(p);
	AssetEntry& e = instance.loadedAssets[id];

	if (e.id == 0)
	{
		Vector2Int16 size;
		TextureId texId = Platform::LoadTexture(p.data(), size);
		e.data = new Texture(p, size, texId);
		e.type = AssetType::Texture;
		e.id = id;
	}
	else
	{
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

	if (e.id == 0)
	{
		e.data = Platform::LoadFont(p.data(), size);
		e.type = AssetType::Font;
		e.id = id;
	}
	else
	{
		if (e.type != AssetType::Font)
			EXCEPTION("Tried to load '%s' as %i but it was %i!",
				p.data(), GetAssetTypeName(AssetType::Font), GetAssetTypeName(e.type));
	}

	return (const Font*)e.data;
}

AudioClip* AssetLoader::LoadAudioClip(const char* path)
{
	std::string p = path;
	AssetId id = instance.hasher(p);
	AssetEntry& e = instance.loadedAssets[id];

	if (e.id == 0)
	{
		e.data = LoadAudioClipFromFile(p.data());
		e.type = AssetType::AudioClip;
		e.id = id;
	}
	else
	{
		if (e.type != AssetType::AudioClip)
			EXCEPTION("Tried to load '%s' as %i but it was %i!",
				p.data(), GetAssetTypeName(AssetType::AudioClip), GetAssetTypeName(e.type));
	}

	return   (AudioClip*)e.data;
}


static uint16_t audioClipId = 0;

static AudioClip* LoadAudioClipFromFile(const char* path)
{
	std::string p = path;
	p += ".wav";

	FILE* f = Platform::OpenAsset(p.data());

	if (f == nullptr)
		EXCEPTION("Failed to open asset '%s': errno %i!", p.data());

	AudioInfo info;

	if (!WaveLoader::ReadWAVHeader(f, &info))
		EXCEPTION("Failed to read WAV header in '%s'!", p.data());

	AudioClip* stream = nullptr;

	if (info.GetTotalSize() < 1024 * 200)
	{
		stream = new AudioClip(info, f);

		fclose(f);

	}
	else
	{
		stream = new AudioClip(info, AudioStreamBufferSize, f);
	}

	stream->id = ++audioClipId;

	return stream;
}

class AssetLoaderLoadDatabaseCrt : public Coroutine {

	FILE* f;
	AssetLoader::AssetId id;
	AssetLoader::AssetEntry* e;

	CRT_START()
	{
		f = Platform::OpenAsset(DataFile);
		if (!f)
			EXCEPTION("Failed to load file %s", DataFile);

		CRT_WAIT_FOR(BinaryDataLoader::LoadDatabaseAsync(f, &AssetLoader::instance.db));

		id = AssetLoader::instance.hasher(DataFile);
		e = &AssetLoader::instance.loadedAssets[id];

		e->id = id;
		e->type = AssetLoader::AssetType::Database;
		e->data = AssetLoader::instance.db;

		CRT_WAIT_FOR(AssetLoader::instance.db->LoadAssetReferencesAsync());

		fclose(f);

	}
	CRT_END();
};

Coroutine* AssetLoader::LoadDatabaseAsync()
{
	return new AssetLoaderLoadDatabaseCrt();
}


class AssetLoaderIOCrt : public Coroutine {
private:
	AssetLoader::IORequest _request;

public:
	AssetLoaderIOCrt(std::function<void()> func)
	{
		_request.completed = false;
		_request.action = func;

		if (AssetLoader::instance._usesIOThread)
		{
			AssetLoader::instance._ioQueue.Enqueue(&_request);
			Platform::ReleaseSemaphore(AssetLoader::instance._semaphore, 1);
		}
	}


	CRT_START()
	{
		if (!AssetLoader::instance._usesIOThread)
		{
			_request.action();
			_request.completed = true;

			CRT_BREAK();
		}

		while (!_request.completed)
		{
			CRT_YIELD();
		}
	}
	CRT_END();
};


Coroutine* AssetLoader::RunIOAsync(std::function<void()> func)
{
	return new AssetLoaderIOCrt(func);
}