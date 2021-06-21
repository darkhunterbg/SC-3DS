#include "UnitDef.h"
#include "GameDatabase.h"

const SpriteDef& UnitDef::UnitArt::GetSprite() const
{
	return GameDatabase::instance->SpriteDefs[SpriteId];
}

const ImageFrame& UnitDef::UnitArt::GetIcon() const
{
	return GameDatabase::instance->GetCommandIcons(IconId);
}


const Image* UnitDef::UnitArt::GetShadowImage() const
{
	if (ShadowImageId < 0)
		return nullptr;
	
	return &GameDatabase::instance->GetImage(ShadowImageId);
}
