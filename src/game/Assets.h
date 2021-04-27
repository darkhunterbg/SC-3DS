#pragma once

#include "MathLib.h"
#include "Span.h"
#include <vector>
#include <stdint.h>
#include <string>
#include <array>

struct Sprite {
	void* textureId;
	Rectangle rect;

	template <class TTextureType>
	TTextureType* GetTextureId() const { return reinterpret_cast<TTextureType*>(textureId); }
};

class SpriteAtlas {
private:
	std::vector<Sprite> sprites;
public:
	inline Span<Sprite> GetSprites() const { return { sprites.data(),sprites.size() }; }
	inline const Sprite& GetSprite(int pos) const { return sprites[pos]; };

	SpriteAtlas(int initialSize = 0);

	void AddSprite(const Sprite& sprite);
};


struct Font {
	void* fontId;

	template <class TFontType>
	TFontType* GetFontId() const { return reinterpret_cast<TFontType*>(fontId); }

	inline bool IsValid() { return fontId != nullptr; }
};