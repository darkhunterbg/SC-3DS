#include "AssetDataDefs.h"
#include "GameDatabase.h"

const Span<AnimClipDef> SpriteDef::GetClips() const {

	return { &GameDatabase::instance->AnimClipDefs[animStart], animCount };
}

const Image& SpriteDef::GetImage() const
{
	return GameDatabase::instance->GetImage(imageId);
}
