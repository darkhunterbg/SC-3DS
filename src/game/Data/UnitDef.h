#pragma once

#include <string>
#include <functional>

#include "../Assets.h"
#include "../MathLib.h"
#include "WeaponDef.h"
#include "UpgradeDef.h"
#include "UnitPortraitDef.h"

#include <cstring>

struct SpriteDef;
struct UnitWireframeDef;
struct SoundSetDef;

typedef uint32_t UnitId;

enum class UnitAIType : uint8_t {
	None = 0,
	Aggressive = 1,
};

#pragma pack(push,2)

struct UnitAttack {
	int16_t WeaponId;
	int16_t MinRange;
	int16_t MaxRange;
	uint8_t Cooldown;
	uint8_t Padding;
	uint16_t Damage;

	const WeaponDef* GetWeapon() const;
	inline bool IsValid() const { return WeaponId >= 0 && Damage > 0; }

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

	std::array<const SoundSetDef*, 4> GetSounds() const;
};


struct UnitDef {
	char Path[32];
	UnitId Id;

	struct UnitText {
		char Name[32];
		char Title[32];

		inline bool HasTitle() const
		{
			return  Title[0] != 0;
		}
	} Text;

	struct UnitWireframe {
		int16_t ImageId;
	};

	struct UnitArt {
		uint16_t IconId;
		uint16_t SpriteId;
		uint16_t BarSize;
		uint16_t BarOffset;
		uint16_t SelectionSize;
		int16_t SelectionOffset;
		int16_t ShadowImageId;
		Vector2Int16 ShadowOffset;
		int16_t PortraitId;
		int16_t WireframeId;


		const UnitWireframeDef* GetWireframe() const;
		const SpriteDef& GetSprite() const;
		const ImageFrame& GetIcon() const;
		const Image* GetShadowImage() const;
		const Image& GetSelectionImage() const;
		const UnitPortraitDef* GetPortrait() const;

	} Art;

	struct UnitCombat {
		uint16_t Armor;
	} Combat;

	UnitAttack Attacks[2];

	struct UnitStats {
		uint16_t Health;
		uint16_t Sheild;
		uint16_t Vision;
		uint16_t UseSupply;
		uint16_t ProvideSupply;
	} Stats;

	struct UnitMovement {
		uint16_t MaxVelocity;
		uint16_t RotationSpeed;
	} Movement;

	struct UnitTechTree {
		int16_t ArmorUpgrateId;
		int16_t ShieldUpgradeId;
		const UpgradeDef* GetArmorUpgrade() const;
		const UpgradeDef* GetShieldUpgrade() const;
	} TechTree;

	UnitSounds Sounds;

	struct UnitAI {
		UnitAIType AIType;
		uint8_t SeekRange;
	} AI;

	Span<UnitAttack> GetAttacks() const;
	inline bool HasMovement() const
	{
		return Movement.MaxVelocity > 0;
	}
};

#pragma pack(pop)