#pragma once

#include <string>
#include <functional>

#include "../Assets.h"
#include "../MathLib.h"
#include "SoundDef.h"
#include "WeaponDef.h"

#include <cstring>

struct UnitDef {
	char Path[32];


	struct {
		uint16_t IconId;
		uint16_t SpriteId;
		char Name[32];
		char Title[32];
		uint16_t BarSize;
		uint16_t BarOffset;
		uint16_t SelectionSize;
		int16_t SelectionOffset;
		int16_t ShadowImageId;
		Vector2Int16 ShadowOffset;

	/*	inline const Image* GetShadowImage() const {
			if (ShadowImageId < 0)
				return nullptr;
			return
		}*/

		inline bool HasTitle() const {
			return  Title[0] != 0;
		}

	} Art;

	uint16_t Health;

	/*

	uint16_t Id = 0;
	std::string Name;
	std::string Title;
	uint16_t Health = 100;
	uint8_t MovementSpeed = 1;
	uint8_t Acceleration = 0;
	uint8_t RotationSpeed = 1;
	uint8_t Vision = 1;
	uint8_t Armor = 0;
	uint16_t BuildTime = 0;

	uint16_t MineralCost = 0;

	bool IsBuilding = false;
	bool IsResourceContainer = false;
	bool IsResourceDepot = false;
	bool GathersResources = false;

	uint8_t UseSupplyDoubled = 0;
	uint8_t ProvideSupplyDoubled = 0;
	uint16_t AudioPriority = 0;

	struct {
		UnitSound Ready;
		UnitSound Yes;
		UnitSound What;
		UnitSound Annoyed;
		UnitSound Death;
	} Sounds;

	
	const WeaponDef* Weapon = nullptr;

	ImageFrame ArmorIcon;
	ImageFrame Portrait;
	ImageFrame Icon;
	int ArmorIconId = -1;
	int PortraitId;
	int IconId;


	Vector2Int16 SpawnOffset;

	const UnitDef* ProductionUnit = nullptr;

	UnitDef() {  }
	UnitDef(const UnitDef&) = delete;
	UnitDef& operator=(const UnitDef&) = delete;

	inline void SetUseSupply(uint8_t amount, bool half = false) {
		UseSupplyDoubled = (amount << 1) + half;
	}
	inline void SetPovideSupply(uint8_t amount, bool half = false) {
		ProvideSupplyDoubled = (amount << 1) + half;
	}
	inline bool HasArmor() const { return ArmorIconId != -1; }


	inline void LoadAllSounds() {

		UnitSound* s = (UnitSound*)&Sounds;
		for (int i = 0; i < 5; ++i) {
			if (s[i].TotalClips > 0)
				s[i].LoadSoundClips();
		}
	}
	*/
};