#include "GraphicsRenderer.h"

#include "../Platform.h"

GraphicsRenderer::GraphicsRenderer()
{
}

void GraphicsRenderer::BufferDraw(std::vector<BatchDrawCommand>& cmd)
{

	//Platform::BatchDraw({ cmd.data(),cmd.size() });
	vertexBuffer.clear();

	if (drawCommands.size() == 0)
		drawCommands.push_back({ cmd.front().sprite.image.textureId , 0,0});

	DrawCommand* dc = &drawCommands.front();

	for (const auto& c : cmd) {

		Vector2Int16 size = c.sprite.rect.size;
		size.x *= c.scale.x;
		size.y *= c.scale.y;

		
		if (dc->texture != c.sprite.image.textureId) {
			
			drawCommands.push_back({ c.sprite.image.textureId, (uint16_t)vertexBuffer.size(),0});
			dc = &drawCommands.back();
		}

		vertexBuffer.push_back({ Vector2(c.position) ,{0,0}, c.color });
		vertexBuffer.push_back({ Vector2(c.position + Vector2Int16(size.x,0)) , {1,0}, c.color });
		vertexBuffer.push_back({ Vector2(c.position + size) , {1,1}, c.color });
		vertexBuffer.push_back({ Vector2(c.position + Vector2Int16(0,size.y)) , {0,1}, c.color });

		dc->count += 4;
	}

	texture = drawCommands.front().texture;

	Submit();
}

void GraphicsRenderer::Submit()
{
	if (!drawCommands.size())
		return;

	Platform::SetDrawBuffers({ vertexBuffer.data(), vertexBuffer.size() });
	Platform::ExecDrawCommands({ drawCommands.data(),drawCommands.size() });

	texture = nullptr;
	drawCommands.clear();
	vertexBuffer.clear();
}
