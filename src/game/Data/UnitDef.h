#pragma once

#include <string>
#include <functional>

#include "../Assets.h"
#include "../MathLib.h"
#include "SoundDef.h"
#include "WeaponDef.h"
#include "UpgradeDef.h"

#include <cstring>

struct SpriteDef;
struct UnitWireframeDef;
struct SoundSetDef;

typedef uint32_t UnitId;


#pragma pack(push,2)

struct UnitAttack {
	int16_t WeaponId;
	Vector2Int16 Range;

	const WeaponDef* GetWeapon() const;
	inline bool IsValid() const { return WeaponId >= 0; }

};

struct UnitSounds {
	int16_t ReadySoundId;
	int16_t YesSoundId;
	int16_t WhatSoundId;
	int16_t AnnoyedSoundId;

	const SoundSetDef* GetReadySound() const;
	const SoundSetDef* GetYesSound() const;
	const SoundSetDef* GetWhatSound() const;
	const SoundSetDef* GetAnnoyedSound() const;
};


struct UnitDef {
	char Path[32];
	UnitId Id;

	struct UnitText {
		char Name[32];
		char Title[32];

		inline bool HasTitle() const {
			return  Title[0] != 0;
		}
	} Text;

	struct UnitWireframe {
		int16_t ImageId;
	};

	struct  UnitArt {
		uint16_t IconId;
		uint16_t SpriteId;
		uint16_t BarSize;
		uint16_t BarOffset;
		uint16_t SelectionSize;
		int16_t SelectionOffset;
		int16_t ShadowImageId;
		Vector2Int16 ShadowOffset;
		int16_t PortraitImageId;
		int16_t WireframeId;


		const UnitWireframeDef* GetWireframe() const;
		const SpriteDef& GetSprite() const;
		const ImageFrame& GetIcon() const;
		const Image* GetShadowImage() const;
		const Image& GetSelectionImage() const;
		const Image* GetPortraitImage() const;

	} Art;

	struct UnitCombat {
		uint16_t Armor;
	} Combat;

	UnitAttack Attacks[2];

	struct UnitStats {
		uint16_t Health;
	} Stats;

	struct UnitMovement {
		uint16_t MaxVelocity;
		uint16_t RotationSpeed;
	} Movement;

	struct UnitTechTree {
		int16_t ArmorUpgrateId;

		const UpgradeDef* GetArmorUpgrade() const;
	} TechTree;

	UnitSounds Sounds;

	Span<UnitAttack> GetAttacks() const;
	inline bool HasMovement() const {
		return Movement.MaxVelocity > 0;
	}


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

#pragma pack(pop)