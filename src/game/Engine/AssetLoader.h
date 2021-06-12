#pragma once

#include "Assets.h"
#include <functional>
#include <string>
#include <unordered_map>
#include <array>

class AssetLoader {
	typedef std::size_t AssetId;

	enum class AssetType : uint8_t {
		Unknown = 0,
		Texture = 1,
		Font = 2,
		AudioClip = 3,
		AudioStream = 4,
		Database = 5
	};

	std::array< const char*, 6> AssetTypeName = {
		   "Unknown", "Texture", "Font", "AudioClip", "AudioStream", "Database"
	};

	struct AssetEntry {
		AssetId id = 0;
		const void* data;
		AssetType type;
	};

private:

	std::hash<std::string> hasher;

	AssetLoader(const AssetLoader&) = delete;
	AssetLoader& operator=(const AssetLoader&) = delete;
	AssetLoader() {}

	std::unordered_map<AssetId, AssetEntry> loadedAssets;

	static AssetLoader instance;

	static inline constexpr const char* GetAssetTypeName(AssetType type)  {
		if ((unsigned)type > instance.AssetTypeName.size()) {
			return "Invalid";
		}
		return instance.AssetTypeName[(unsigned)type];
	}

public:
	
	static void LoadDatabase();

	static const Texture* LoadTexture(const char* path);
	static const Font* LoadFont(const char* path, int size);
	static AudioClip LoadAudioClip(const char* path);
	static AudioStream* LoadAudioStream(const char* path);

	static const SpriteAtlas* LoadAtlas(const char* path);
};