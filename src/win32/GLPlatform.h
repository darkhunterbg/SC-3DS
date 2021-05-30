#pragma once

#include <SDL.h>
#include <cstdint>

#include "Engine/GraphicsPrimitives.h"
#include "Span.h"

class GLPlatform {

private:
	GLPlatform() = delete;
	~GLPlatform() = delete;

public:
	static void Init();
	static void PrepareDraw();

	static void UpdateBuffer(const Span<Vertex> vertexes);

	static void DrawTriangles(unsigned start, unsigned count);
	static void DrawLines(unsigned start, unsigned count);
};