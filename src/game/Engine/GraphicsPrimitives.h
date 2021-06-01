#pragma once

#include "MathLib.h"
#include "Color.h"

typedef void* Texture;
typedef void* Surface;

struct Sprite {
	Rectangle16 rect;
	Vector2 uv[4];
	Texture textureId;

	inline Rectangle GetRect() const {
		return { Vector2Int(rect.position), Vector2Int(rect.size) };
	}
};

struct Vertex {
	Vector2 position;
	Vector2 uv;
	Color32 color;
};

enum class DrawCommandType : uint8_t {
	None,
	TexturedTriangle,
	Triangle,
};

struct DrawCommand {
	Texture texture;
	uint16_t start;
	uint16_t count;
	DrawCommandType type;
};


struct BatchDrawCommand {
	int order;
	Sprite sprite;
	Vector2Int16 position;
	Vector2 scale;
	Color32 color;
};


enum class ScreenId
{
	Top = 0,
	Bottom = 1,
};

enum class BlendMode {
	Alpha = 0,
	AlphaSet = 1,
	AllSet = 2,
};


struct RenderSurface {
	Surface surfaceId = nullptr;
	Sprite sprite;

	inline Vector2Int GetSize() const {
		return Vector2Int(sprite.rect.size);
	}
	inline Rectangle GetRect() const {
		return sprite.GetRect();
	}

};