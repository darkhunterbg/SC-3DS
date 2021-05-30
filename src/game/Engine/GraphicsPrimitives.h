#pragma once

#include "MathLib.h"
#include "Color.h"

typedef void* Texture;

struct Vertex {
	Vector2 position;
	Vector2 uv;
	Color32 color;
};


enum class DrawPrimitiveType : uint8_t {
	Triangle,
	Line
};

struct DrawCommand {
	Texture texture;
	uint16_t start;
	uint16_t count;
};