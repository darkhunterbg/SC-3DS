#include "UnitDef.h"
#include "GameDatabase.h"

const SpriteDef& UnitDef::UnitArt::GetSprite() const
{
	return GameDatabase::instance->SpriteDefs[SpriteId];
}

const ImageFrame& UnitDef::UnitArt::GetIcon() const
{
	return GameDatabase::instance->GetIcon(IconId);
}

const Image* UnitDef::UnitArt::GetShadowImage() const
{
	if (ShadowImageId < 0)
		return nullptr;
	
	return &GameDatabase::instance->GetImage(ShadowImageId);
}

const Image* UnitDef::UnitArt::GetPortraitImage() const
{
	if (PortraitImageId < 0)
		return nullptr;

	return &GameDatabase::instance->GetImage(PortraitImageId);
}

const Image& UnitDef::UnitArt::GetSelectionImage() const
{
	return GameDatabase::instance->GetSelectionImage(SelectionSize);
}

const UnitWireframeDef* UnitDef::UnitArt::GetWireframe() const
{
	if (WireframeId < 0)
		return nullptr;

	return &GameDatabase::instance->WireframeDefs[WireframeId];
}

const UpgradeDef* UnitDef::UnitTechTree::GetArmorUpgrade() const
{
	if (ArmorUpgrateId < 0)
		return nullptr;

	return &GameDatabase::instance->UpgradeDefs[ArmorUpgrateId];
}

const WeaponDef* UnitAttack::GetWeapon() const
{
	if (WeaponId < 0)
		return nullptr;

	return &GameDatabase::instance->WeaponDefs[WeaponId];
}

Span<UnitAttack> UnitDef::GetAttacks() const
{
	if (Attacks[0].IsValid()) {
		if (Attacks[1].IsValid())
			return{ Attacks, 2 };

		return { Attacks,1 };
	}

	if (Attacks[1].IsValid())
		return { Attacks + 1, 1 };

	return { Attacks,0 };
}
