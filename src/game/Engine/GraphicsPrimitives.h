#pragma once

#include "MathLib.h"
#include "Color.h"
#include "../Assets.h"

struct ImageFrameDef;

typedef void* SurfaceId;

enum class DrawCommandType : uint8_t {
	None,
	TexturedTriangle,
	Triangle,
};

struct BatchDrawCommand {
	int order;
	const ImageFrame* sprite;
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
	SurfaceId surfaceId = nullptr;
	ImageFrame sprite;

	inline Vector2Int GetSize() const {
		return Vector2Int(sprite.size);
	}
	inline Rectangle GetRect() const {
		return sprite.GetRect();
	}

};