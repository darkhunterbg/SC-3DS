#include "GraphicsDef.h"

#include "../Engine/AssetLoader.h"

void SpriteAtlasDef::Load()
{
	Atlas = AssetLoader::LoadAtlas(Path);
}
