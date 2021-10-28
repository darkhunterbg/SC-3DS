#include "AssetLoader.h"
#include "Platform.h"
#include "Debug.h"
#include "../Loader/Wave.h"
#include "../Loader/BinaryData.h"
#include "../Loader/smacker.h"

static AudioClip* LoadAudioClipFromFile(const char* path);
static Texture* LoadTextureFromFile(const char* path);
static VideoClip* LoadVideoClipFromFile(const char* path);

static constexpr const char* DataFile = "data";

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
			GAME_ASSERT(!request->completed, "FATAL ERROR: ProcessIORequests got request->completed = true!");

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

Texture* AssetLoader::LoadTexture(const char* path)
{
	std::string p = path;
	AssetId id = instance.hasher(p);
	AssetEntry& e = instance.loadedAssets[id];

	if (e.id == 0)
	{
		Vector2Int16 size;
		e.data = LoadTextureFromFile(path);
		e.type = AssetType::Texture;
		e.id = id;
		((Texture*)e.data)->Id = id;
	}
	else
	{
		if (e.type != AssetType::Texture)
			EXCEPTION("Tried to load '%s' as %i but it was %i!",
				p.data(), GetAssetTypeName(AssetType::Texture), GetAssetTypeName(e.type));
	}

	return (Texture*)e.data;
}

void AssetLoader::UnloadTexture(Texture* texture)
{
	if (!texture) return;

	AssetEntry& e = instance.loadedAssets[texture->Id];

	GAME_ASSERT(e.id != 0, "Failed to find textured with id %i to delete", texture->Id);

	instance.loadedAssets.erase(e.id);

	delete texture;
}

Font* AssetLoader::LoadFont(const char* path, int size)
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

	return (Font*)e.data;
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

	return (AudioClip*)e.data;
}

VideoClip* AssetLoader::LoadVideoClip(const char* path)
{
	std::string p = path;
	AssetId id = instance.hasher(p);
	AssetEntry& e = instance.loadedAssets[id];

	if (e.id == 0)
	{
		e.data = LoadVideoClipFromFile(p.data());
		e.type = AssetType::VideoClip;
		e.id = id;
		((VideoClip*)e.data)->Id = id;
	}
	else
	{
		if (e.type != AssetType::VideoClip)
			EXCEPTION("Tried to load '%s' as %i but it was %i!",
				p.data(), GetAssetTypeName(AssetType::VideoClip), GetAssetTypeName(e.type));
	}

	return   (VideoClip*)e.data;
}
void AssetLoader::UnloadVideoClip(VideoClip* clip)
{
	GAME_ASSERT(clip, "Tried to unload nullptr clip!");

	if (!clip) return;

	AssetEntry& e = instance.loadedAssets[clip->Id];

	GAME_ASSERT(e.id != 0, "Failed to find video with id %i to delete", clip->Id);

	instance.loadedAssets.erase(e.id);

	delete clip;
}
static VideoClip* LoadVideoClipFromFile(const char* path)
{
	std::string p = path;

	FILE* f = Platform::OpenAsset(p.data(), AssetType::VideoClip);

	if (f == nullptr)
		EXCEPTION("Failed to open asset '%s'!", p.data());

	smk handle = smk_open_filepointer(f, SMK_MODE_DISK);
	if (handle == nullptr)
		EXCEPTION("Failed to load SMK video '%s'!", p.data());

	return new VideoClip(handle);

}
static uint16_t audioClipId = 0;
static AudioClip* LoadAudioClipFromFile(const char* path)
{
	std::string p = path;

	FILE* f = Platform::OpenAsset(p.data(), AssetType::AudioClip);

	if (f == nullptr)
		EXCEPTION("Failed to open asset '%s'!", p.data());

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
		stream = new AudioClip(info, f);
	}

	stream->id = ++audioClipId;

	return stream;
}
static Texture* LoadTextureFromFile(const char* path)
{
	std::string p = path;

	FILE* f = Platform::OpenAsset(p.data(), AssetType::Texture);

	setvbuf(f, NULL, _IOFBF, 64 * 1024);

	if (f == nullptr)
		EXCEPTION("Failed to open asset '%s'!", p.data());

	fseek(f, 0, SEEK_END);
	unsigned size = ftell(f);
	fseek(f, 0, SEEK_SET);

	uint8_t* data = new uint8_t[size];

	auto read = fread(data, sizeof(uint8_t), size, f);
	if (read != size) EXCEPTION("Failed to read data from asset '%s'", p.data());

	fclose(f);

	Vector2Int16 texSize;
	TextureId texId = Platform::CreateTextureFromFile(p.data(), { data, size }, texSize);

	delete[] data;

	return new Texture(p, texSize, texId);
}
class AssetLoaderLoadDatabaseCrt : public CoroutineImpl {

	FILE* f;
	AssetId id;
	AssetLoader::AssetEntry* e;
	CoroutineR<GameDatabase*> loadDBCrt;
	CRT_START()
	{
		f = Platform::OpenAsset(DataFile, AssetType::Database);
		if (!f)
			EXCEPTION("Failed to load file %s", DataFile);

		loadDBCrt = BinaryDataLoader::LoadDatabaseAsync(f);
		CRT_WAIT_FOR(loadDBCrt);
		AssetLoader::instance.db = loadDBCrt->GetResult();

		id = AssetLoader::instance.hasher(DataFile);
		e = &AssetLoader::instance.loadedAssets[id];

		e->id = id;
		e->type = AssetType::Database;
		e->data = AssetLoader::instance.db;

		CRT_WAIT_FOR(AssetLoader::instance.db->LoadAssetReferencesAsync());

		fclose(f);

	}
	CRT_END();
};

Coroutine AssetLoader::LoadDatabaseAsync()
{
	return  Coroutine(new AssetLoaderLoadDatabaseCrt());
}


class AssetLoaderIOCrt : public CoroutineImpl {
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


Coroutine AssetLoader::RunIOAsync(std::function<void()> func)
{
	return Coroutine(new AssetLoaderIOCrt(func));
}