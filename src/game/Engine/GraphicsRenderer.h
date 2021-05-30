#pragma once


#include "../Platform.h"
#include "GraphicsPrimitives.h"

#include <vector>


class GraphicsRenderer {
private:

	std::vector<Vertex> vertexBuffer;
	Texture texture = nullptr;

public:
	GraphicsRenderer();
	GraphicsRenderer(const GraphicsRenderer&) = delete;
	GraphicsRenderer& operator=(const GraphicsRenderer&) = delete;

	void BufferDraw(std::vector<BatchDrawCommand>& cmd);
};