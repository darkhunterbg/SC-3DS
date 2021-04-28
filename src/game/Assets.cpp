#include "Assets.h"


SpriteAtlas::SpriteAtlas(int initialSize ) {
	sprites.reserve(initialSize);
}

void SpriteAtlas::AddSprite(const Sprite& sprite) {
	sprites.push_back(sprite);
}
