#include "GraphicsRenderer.h"

#include "../Platform.h"



void GraphicsRenderer::Init()
{
	vertexBuffer = Platform::GetVertexBuffer();
}

void GraphicsRenderer::AddVertex(const Vector2& pos, const Vector2& uv, const Color32& c)
{
	auto& v = vertexBuffer[vbPos++];
	v.position = pos;
	v.uv = uv;
	v.color = c;
}

void GraphicsRenderer::BufferDraw(std::vector<BatchDrawCommand>& cmd)
{

	//Platform::BatchDraw({ cmd.data(),cmd.size() });
	vbPos = 0;

	if (drawCommands.size() == 0)
		drawCommands.push_back({ cmd.front().sprite.image.textureId , 0,0 });


	DrawCommand* dc = &drawCommands.front();

	Vertex* vb = vertexBuffer;
	//return;

	for (const auto& c : cmd) {

		Vector2Int16 size = c.sprite.rect.size;
		size.x *= c.scale.x;
		size.y *= c.scale.y;

		//f += 2;

		Vector2 start = c.sprite.uv[0];
		Vector2 end = c.sprite.uv[1];
		/*	Vector2 start = Vector2(c.sprite.rect.GetMin()) / Vector2(512.f, 512.f);
			Vector2 end = Vector2(c.sprite.rect.GetMax()) / Vector2(512.f, 512.f);*/

		if (dc->texture != c.sprite.image.textureId) {

			drawCommands.push_back({ c.sprite.image.textureId, (uint16_t)vbPos,0 });
			dc = &drawCommands.back();
		}


		AddVertex(Vector2(c.position), start, c.color);
		AddVertex(Vector2(c.position + Vector2Int16(size.x, 0)), { end.x,start.y }, c.color);
		AddVertex(Vector2(c.position + size), end, c.color);

		AddVertex(Vector2(c.position + size), end, c.color);
		AddVertex(Vector2(c.position + Vector2Int16(0, size.y)), { start.x,end.y }, c.color);
		AddVertex(Vector2(c.position), start, c.color);

		/*	vb[vbPos++] = { Vector2(c.position) ,start, c.color };
			vb[vbPos++] = { Vector2(c.position + Vector2Int16(size.x,0)) , {end.x,start.y}, c.color };
			vb[vbPos++] = { Vector2(c.position + size) , end, c.color };

			vb[vbPos++] = { Vector2(c.position + size) , end, c.color };
			vb[vbPos++] = { Vector2(c.position + Vector2Int16(0,size.y)) ,{start.x,end.y}, c.color };
			vb[vbPos++] = { Vector2(c.position) ,start, c.color };*/


		dc->count += 6;
	}

	texture = drawCommands.front().texture;

	Submit();
}

void GraphicsRenderer::Submit()
{
	if (!drawCommands.size())
		return;

	Platform::ExecDrawCommands({ drawCommands.data(), drawCommands.size() });

	texture = nullptr;
	drawCommands.clear();
	vbPos = 0;
}
