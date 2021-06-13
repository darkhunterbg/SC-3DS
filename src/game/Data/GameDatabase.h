#pragma once

#include <vector>

#include "AssetDataDefs.h"
#include "../Assets.h"
#include <algorithm>
#include <unordered_map>

class GameDatabase {
private:
	std::vector<ImageFrame> frames;
	std::vector<Image> images;

	std::unordered_map<std::string, uint16_t> imageNamesMap;
public:
	std::vector<AtlasDef> AtlasDefs;
	std::vector<ImageDef> ImageDefs;
	std::vector<ImageFrameDef> FrameDefs;
	std::vector<SpriteDef> SpriteDefs;
	std::vector<AnimClipDef> AnimClipDefs;
	std::vector<AnimInstructionDef> AnimInstructionDefs;

	GameDatabase(){}
	GameDatabase(const GameDatabase&) = delete;
	GameDatabase& operator=(const GameDatabase&) = delete;


	void LoadAssetReferences();
};