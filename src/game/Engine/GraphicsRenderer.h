#pragma once


#include "../Platform.h"
#include "GraphicsPrimitives.h"

#include <vector>

#include <Span.h>


class GraphicsRenderer {

private:

	std::vector<DrawCommand> drawCommands;
	Vertex* vertexBuffer;
	unsigned vbPos = 0;
	Texture texture = nullptr;

	void AddVertex(const Vector2& pos, const Vector2& uv, const Color32& c);
public:
	void Init();

	GraphicsRenderer() {}
	GraphicsRenderer(const GraphicsRenderer&) = delete;
	GraphicsRenderer& operator=(const GraphicsRenderer&) = delete;

	void BufferDraw	(std::vector<BatchDrawCommand>& cmd);



	void Submit();
};