#pragma once

#include "Assets.h"
#include <functional>
#include <string>
#include <unordered_map>
#include <array>
#include "../Data/GameDatabase.h"
#include "../Coroutine.h"
#include "../RingBuffer.h"

typedef void* Semaphore;


class AssetLoader {
	friend class AssetLoaderLoadDatabaseCrt;
	friend class AssetLoaderIOCrt;

	typedef std::size_t AssetId;

	enum class AssetType : uint8_t {
		Unknown = 0,
		Texture = 1,
		Font = 2,
		AudioClip = 3,
		Database = 4
	};

	std::array< const char*, 5> AssetTypeName = {
		   "Unknown", "Texture", "Font", "AudioClip", "Database"
	};

	struct AssetEntry {
		AssetId id = 0;
		const void* data;
		AssetType type;
	};

	struct IORequest {
		std::function<void(void)> action;
		volatile bool completed = false;
	};


private:

	std::hash<std::string> hasher;
	GameDatabase* db = nullptr;
	RingBuffer<IORequest*, 1024> _ioQueue;
	Semaphore _semaphore = 0;
	bool _usesIOThread = false;

	AssetLoader(const AssetLoader&) = delete;
	AssetLoader& operator=(const AssetLoader&) = delete;
	AssetLoader() {}

	std::unordered_map<AssetId, AssetEntry> loadedAssets;

	static AssetLoader instance;

	static void ProcessIORequests(int threadId);

	static inline constexpr const char* GetAssetTypeName(AssetType type)
	{
		if ((unsigned)type > instance.AssetTypeName.size())
		{
			return "Invalid";
		}
		return instance.AssetTypeName[(unsigned)type];
	}


public:
	inline static GameDatabase& GetDatabase()
	{
		return *instance.db;
	}

	static void Init();

	static Coroutine LoadDatabaseAsync();

	static const Texture* LoadTexture(const char* path);
	static const Font* LoadFont(const char* path, int size);
	static AudioClip* LoadAudioClip(const char* path);

	static Coroutine RunIOAsync(std::function<void()> func);
};