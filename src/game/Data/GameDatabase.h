#pragma once

#include <vector>

#include "AssetDataDefs.h"
#include "UnitDef.h"
#include "AbilityDef.h"
#include "../Assets.h"
#include <algorithm>
#include <unordered_map>

class GameDatabase {
private:
	std::vector<ImageFrame> frames;
	std::vector<Image> images;

	const Image* commandIcons;

	std::unordered_map<std::string, const Image*> imageNamesMap;
	std::unordered_map<UnitId, const UnitDef*> unitMap;
	std::unordered_map<AbilityId, const AbilityDef*> abilityMap;
public:
	static const GameDatabase* instance;

	std::vector<AtlasDef> AtlasDefs;
	std::vector<ImageDef> ImageDefs;
	std::vector<ImageFrameDef> FrameDefs;
	std::vector<SpriteDef> SpriteDefs;
	std::vector<AnimClipDef> AnimClipDefs;
	std::vector<AnimInstructionDef> AnimInstructionDefs;
	std::vector<UnitDef> UnitDefs;
	std::vector<AbilityDef> AbilityDefs;

	GameDatabase();
	GameDatabase(const GameDatabase&) = delete;
	GameDatabase& operator=(const GameDatabase&) = delete;

	inline const Image& GetImage(unsigned id) const { return images[id]; }
	const Image& GetImage(const std::string& path) const;
	const ImageFrame& GetCommandIcons(unsigned id) const;

	const UnitDef* GetUnit(const char* path) const;
	const UnitDef* GetUnit(UnitId id) const;

	const AbilityDef* GetAbility(const char* path) const;
	const AbilityDef* GetAbility(AbilityId id) const;

	void LoadAssetReferences();
};