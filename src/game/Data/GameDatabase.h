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

	const Image* commandIcons;

	std::unordered_map<std::string, const Image*> imageNamesMap;
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

	const Image& GetImage(const std::string& path) const;
	const ImageFrame& GetCommandIcons(unsigned id) const;

	void LoadAssetReferences();
};