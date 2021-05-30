#include "GraphicsRenderer.h"

#include "../Platform.h"

GraphicsRenderer::GraphicsRenderer()
{
}

void GraphicsRenderer::BufferDraw(std::vector<BatchDrawCommand>& cmd)
{
	texture = nullptr;

	//Platform::BatchDraw({ cmd.data(),cmd.size() });
	vertexBuffer.clear();

	for (const auto& c : cmd) {


		vertexBuffer.clear();

		Vector2Int16 size = c.sprite.rect.size;
		size.x *= c.scale.x;
		size.y *= c.scale.y;

		//vertexBuffer.push_back({ {-1,-1}, {0,0}, c.color });
		//vertexBuffer.push_back({ {1,-1},  {1,0}, c.color });
		//vertexBuffer.push_back({ {0,1},  {0,1}, c.color });

		vertexBuffer.push_back({ Vector2(c.position) ,{0,0}, c.color });
		vertexBuffer.push_back({ Vector2(c.position + Vector2Int16(size.x,0)) , {1,0}, c.color });
		vertexBuffer.push_back({ Vector2(c.position + size) , {1,1}, c.color });
		vertexBuffer.push_back({ Vector2(c.position) ,{0,0}, c.color });
		vertexBuffer.push_back({ Vector2(c.position + size) , {1,1}, c.color });
		vertexBuffer.push_back({ Vector2(c.position + Vector2Int16(0,size.y)) , {0,1}, c.color });

		texture = c.sprite.image.textureId;

		Platform::SetBuffer({ vertexBuffer.data(), vertexBuffer.size() });
		Platform::DrawBuffer(0, vertexBuffer.size(), texture);
	}

	
}
