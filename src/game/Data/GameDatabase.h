#pragma once

#include <vector>

#include "AssetDataDefs.h"
#include "UnitDef.h"
#include "AbilityDef.h"
#include "UpgradeDef.h"
#include "SoundSetDef.h"
#include "RaceDef.h"

#include "../Assets.h"
#include <algorithm>
#include <unordered_map>


class Coroutine;

class GameDatabase {
private:
	friend class GameDatabaseLoadAsetReferencesCrt;

	std::vector<ImageFrame> frames;
	std::vector<Image> images;
	std::vector<AudioClip*> audioClips;

	const Image* commandIcons;
	std::vector<const Image*> selections;

	std::unordered_map<std::string, const Image*> imageNamesMap;
	std::unordered_map<UnitId, const UnitDef*> unitMap;
	std::unordered_map<AbilityId, const AbilityDef*> abilityMap;

	void CreateRaces();
public:
	static const GameDatabase* instance;

	std::vector<AtlasDef> AtlasDefs;
	std::vector<ImageDef> ImageDefs;
	std::vector<ImageFrameDef> FrameDefs;
	std::vector<AudioClipDef> AudioClipDefs;
	std::vector<SpriteDef> SpriteDefs;
	std::vector<AnimClipDef> AnimClipDefs;
	std::vector<AnimInstructionDef> AnimInstructionDefs;
	std::vector<UnitDef> UnitDefs;
	std::vector<AbilityDef> AbilityDefs;
	std::vector<UnitWireframeDef> WireframeDefs;
	std::vector<UpgradeDef> UpgradeDefs;
	std::vector<WeaponDef> WeaponDefs;
	std::vector<SoundSetDef> SoundSetDefs;

	std::vector<RaceDef> RaceDefs;

	const AbilityDef* MoveAbility = nullptr;
	const AbilityDef* AttackAbility = nullptr;
	const AbilityDef* StopAbility = nullptr;
	const AbilityDef* PatrolAbility = nullptr;
	const AbilityDef* HoldPositionAbility = nullptr;

	GameDatabase();
	GameDatabase(const GameDatabase&) = delete;
	GameDatabase& operator=(const GameDatabase&) = delete;

	inline const Image& GetImage(unsigned id) const { return images[id]; }
	inline const Image& GetSelectionImage(unsigned id) const { return *selections[id]; }


	const Image& GetImage(const std::string& path) const;
	const ImageFrame& GetIcon(unsigned id) const;

	const UnitDef* GetUnit(const char* path) const;
	const UnitDef* GetUnit(UnitId id) const;

	const AbilityDef* GetAbility(const char* path) const;
	const AbilityDef* GetAbility(AbilityId id) const;

	const SoundSetDef* GetSoundSet(const char* path) const;
	const RaceDef* GetRace(RaceType type) const;

	inline AudioClip& GetAudioClip(unsigned id) const { return *audioClips[id]; }
	inline Span<AudioClip*> GetAudioClips(unsigned id, unsigned count) const {
		return { &audioClips[id], count };
	}

	Coroutine* LoadAssetReferencesAsync();
};