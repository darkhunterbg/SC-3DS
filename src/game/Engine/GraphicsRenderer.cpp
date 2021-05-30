#include "GraphicsRenderer.h"

#include "../Platform.h"

GraphicsRenderer::GraphicsRenderer()
{
}

void GraphicsRenderer::BufferDraw(std::vector<BatchDrawCommand>& cmd)
{

	//Platform::BatchDraw({ cmd.data(),cmd.size() });
	vertexBuffer.clear();

	for (const auto& c : cmd) {
		vertexBuffer.push_back({ {-1,-1}, {0,0}, c.color });
		vertexBuffer.push_back({ {1,-1},  {1,0}, c.color });
		vertexBuffer.push_back({ {0,1},  {0,1}, c.color });

		/*	vertexBuffer.push_back({ Vector2(c.position)/ 400.0f, {0,0}, c.color });
			vertexBuffer.push_back({ Vector2(c.position + Vector2Int16(32,0)) / 400.0f, {1,0}, c.color });
			vertexBuffer.push_back({ Vector2(c.position + Vector2Int16(0,30)) / 400.0f, {0,1}, c.color });*/
	}

	Platform::SetBuffer({ vertexBuffer.data(), vertexBuffer.size() });
	Platform::DrawBuffer(0, vertexBuffer.size() / 3, 0);
}
