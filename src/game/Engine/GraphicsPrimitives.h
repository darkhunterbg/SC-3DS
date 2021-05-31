#pragma once

#include "MathLib.h"
#include "Color.h"
#include "../Assets.h"

typedef void* Texture;

struct Vertex {
	Vector2 position;
	Vector2 uv;
	Color32 color;
};

enum class DrawCommandType : uint8_t {
	None,
	TexturedTriangle,
	Triangle,
	Line
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
