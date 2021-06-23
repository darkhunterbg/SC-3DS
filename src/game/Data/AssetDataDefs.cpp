#include "AssetDataDefs.h"
#include "GameDatabase.h"

 const AnimClipDef* SpriteDef::GetAnimation(AnimationType type) const
{
	for (int i = animStart; i < animStart + animCount; ++i) {
		if (GameDatabase::instance->AnimClipDefs[i].type == type)
			return  &GameDatabase::instance->AnimClipDefs[i];
	}
	return nullptr;
}

const Span<AnimClipDef> SpriteDef::GetClips() const {

	return { &GameDatabase::instance->AnimClipDefs[animStart], animCount };
}

const Image& SpriteDef::GetImage() const
{
	return GameDatabase::instance->GetImage(imageId);
}

const Image& UnitWireframeDef::Wireframe::GetImage() const
{
	return GameDatabase::instance->GetImage(imageId);
}

