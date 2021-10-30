#include "AssetLoader.h"
#include "Platform.h"
#include "Debug.h"
#include "../Loader/Wave.h"
#include "../Loader/BinaryData.h"
#include "../Loader/smacker.h"
#include "../Loader/Pak.h"

#include "JobSystem.h"

static Texture* LoadTextureFromFile(const char* path);

static constexpr const char* DataFile = "data";

static std::hash<int> intHasher;

AssetLoader AssetLoader::instance;


void AssetLoader::ProcessIORequests(int threadId)
{
	while (true)
	{
		IORequest* request = nullptr;

		Platform::WaitSemaphore(instance._semaphore);

		Platform::LockMutex(instance._mutex);

		bool hasWork = instance._ioQueue.TryDequeue(&request);

		Platform::UnlockMutex(instance._mutex);

		if (hasWork)
		{
			GAME_ASSERT(!request->completed, "FATAL ERROR: ProcessIORequests got request->completed = true!");

			request->action();
			request->completed = true;
		}


	}
}

void AssetLoader::LoadPack(const char* packFileName)
{
	FILE* f = Platform::OpenAsset(packFileName, AssetType::Unknown);
	GAME_ASSERT(f, "Failed to load %s", packFileName);

	std::vector<PakFileEntry> files;

	PakLoader::LoadEntries(f, files);

	for (const auto& e : files)
	{
		AssetId id = instance.hasher(e.name);
		_assets[id] = PakAssetEntry{ f,e.offset, e.size };
	}

	// Don't close pak file's stream

	//if (f != nullptr)
	//	fclose(f);
}

void AssetLoader::Init()
{
	instance._semaphore = Platform::CreateSemaphore();
	instance._mutex = Platform::CreateMutex();

	instance._usesIOThread = Platform::TryStartThreads(ThreadUsageType::IO, 1, ProcessIORequests) > 0;

	instance.LoadPack("audio.pak");
	instance.LoadPack("video.pak");
}

Texture* AssetLoader::LoadTexture(const char* path)
{
	GAME_ASSERT(IsMainThread(), "LoadTexture should be called only on main thread!");

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
	GAME_ASSERT(IsMainThread(), "UnloadTexture should be called only on main thread!");

	if (!texture) return;

	AssetEntry& e = instance.loadedAssets[texture->Id];

	GAME_ASSERT(e.id != 0, "Failed to find textured with id %i to delete", texture->Id);

	instance.loadedAssets.erase(e.id);

	delete texture;
}

Font* AssetLoader::LoadFont(const char* path, int size)
{
	GAME_ASSERT(IsMainThread(), "LoadFont should be called only on main thread!");

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
	GAME_ASSERT(IsMainThread(), "LoadAudioClip should be called only on main thread!");

	std::string p = path;
	AssetId id = instance.hasher(p);
	AssetEntry& e = instance.loadedAssets[id];

	if (e.id == 0)
	{
		PakAssetEntry& pak = instance._assets[id];

		if (pak.size == 0)
			EXCEPTION("Failed to load audio clip '%s'", path);

		AudioInfo info;

		fseek(pak.file, pak.position, SEEK_SET);

		if (!WaveLoader::ReadWAVHeader(pak.file, &info))
			EXCEPTION("Failed to read WAV header in '%s'!", path);


		e.data = new AudioClip(info, pak.file);
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
	GAME_ASSERT(IsMainThread(), "LoadVideoClip should be called only on main thread!");

	std::string p = path;
	AssetId id = instance.hasher(p);
	AssetEntry& e = instance.loadedAssets[id];

	if (e.id == 0)
	{
		PakAssetEntry& pak = instance._assets[id];

		if (pak.size == 0)
			EXCEPTION("Failed to load audio clip '%s'", path);

		fseek(pak.file, pak.position, SEEK_SET);

		e.data = new VideoClip(pak.file);
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
	GAME_ASSERT(IsMainThread(), "UnloadVideoClip should be called only on main thread!");
	GAME_ASSERT(clip, "Tried to unload nullptr clip!");

	if (!clip) return;

	AssetEntry& e = instance.loadedAssets[clip->Id];

	GAME_ASSERT(e.id != 0, "Failed to find video with id %i to delete", clip->Id);

	instance.loadedAssets.erase(e.id);

	delete clip;
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
			Platform::LockMutex(AssetLoader::instance._mutex);
			AssetLoader::instance._ioQueue.Enqueue(&_request);
			Platform::UnlockMutex(AssetLoader::instance._mutex);

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