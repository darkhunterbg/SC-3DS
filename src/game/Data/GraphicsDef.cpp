#include "GraphicsDef.h"

#include "../Game.h"

void SpriteAtlasDef::Load()
{
	Atlas = Game::AssetLoader.LoadAtlas(Path);
}
