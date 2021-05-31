#include "GraphicsRenderer.h"

#include "../Platform.h"


GraphicsRenderer GraphicsRenderer::instance;

void GraphicsRenderer::Init()
{
	instance.vertexBuffer = Platform::GetVertexBuffer();
	Platform::ChangeBlendingMode(instance.blendMode);
}

void GraphicsRenderer::AddVertex(const Vector2& pos, const Vector2& uv, const Color32& c)
{
	auto& v = vertexBuffer[vbPos++];
	v.position = pos;
	v.uv = uv;
	v.color = c;
}

DrawCommand& GraphicsRenderer::GetDrawCommand(DrawCommandType type)
{
	if (instance.cmdType != type)
	{
		instance.drawCommands.push_back({ texture , (uint16_t)instance.vbPos,0 , type });
		instance.texture = nullptr;
		instance.cmdType = type;
	}
	return instance.drawCommands.back();
}

DrawCommand& GraphicsRenderer::NewDrawCommand(Texture texture)
{
	instance.drawCommands.push_back({ texture , (uint16_t)instance.vbPos,0 ,DrawCommandType::TexturedTriangle });
	instance.texture = texture;
	instance.cmdType = DrawCommandType::TexturedTriangle;

	return instance.drawCommands.back();
}

DrawCommand& GraphicsRenderer::GetDrawCommand(Texture texture)
{
	if (instance.texture != texture)
	{
		instance.drawCommands.push_back({ texture , (uint16_t)instance.vbPos,0 ,DrawCommandType::TexturedTriangle });
		instance.texture = texture;
		instance.cmdType = DrawCommandType::TexturedTriangle;
	}
	return instance.drawCommands.back();
}

void GraphicsRenderer::BufferDraw(std::vector<BatchDrawCommand>& cmd)
{
	if (instance.drawCommands.size() == 0)
		instance.drawCommands.push_back({ cmd.front().sprite.textureId , 0,0 , DrawCommandType::TexturedTriangle });

	DrawCommand* dc = &instance.drawCommands.front();

	for (const auto& c : cmd) {

		Vector2Int16 size = c.sprite.rect.size;
		size.x *= c.scale.x;
		size.y *= c.scale.y;

		Vector2 start = c.sprite.uv[0];
		Vector2 end = c.sprite.uv[1];

		if (size.x < 0) {
			size.x = -size.x;
			float f = start.x;
			start.x = end.x;
			end.x = f;
		}


		if (dc->texture != c.sprite.textureId) {
			dc = &instance.NewDrawCommand(c.sprite.textureId);
		}


		instance.AddVertex(Vector2(c.position), start, c.color);
		instance.AddVertex(Vector2(c.position + Vector2Int16(size.x, 0)), { end.x,start.y }, c.color);
		instance.AddVertex(Vector2(c.position + size), end, c.color);

		instance.AddVertex(Vector2(c.position + size), end, c.color);
		instance.AddVertex(Vector2(c.position + Vector2Int16(0, size.y)), { start.x,end.y }, c.color);
		instance.AddVertex(Vector2(c.position), start, c.color);

		/*	vb[vbPos++] = { Vector2(c.position) ,start, c.color };
			vb[vbPos++] = { Vector2(c.position + Vector2Int16(size.x,0)) , {end.x,start.y}, c.color };
			vb[vbPos++] = { Vector2(c.position + size) , end, c.color };

			vb[vbPos++] = { Vector2(c.position + size) , end, c.color };
			vb[vbPos++] = { Vector2(c.position + Vector2Int16(0,size.y)) ,{start.x,end.y}, c.color };
			vb[vbPos++] = { Vector2(c.position) ,start, c.color };*/


		dc->count += 6;
	}

	instance.texture = instance.drawCommands.front().texture;

}

void GraphicsRenderer::Draw(const Sprite& sprite, Vector2Int position, Color32 color)
{
	instance.GetDrawCommand(sprite.textureId).count += 6;

	Vector2Int size = Vector2Int(sprite.rect.size);

	Vector2 start = sprite.uv[0];
	Vector2 end = sprite.uv[1];

	instance.AddVertex(Vector2(position), start, color);
	instance.AddVertex(Vector2(position + Vector2Int(size.x, 0)), { end.x,start.y }, color);
	instance.AddVertex(Vector2(position + size), end, color);

	instance.AddVertex(Vector2(position + size), end, color);
	instance.AddVertex(Vector2(position + Vector2Int(0, size.y)), { start.x,end.y }, color);
	instance.AddVertex(Vector2(position), start, color);
}

void GraphicsRenderer::Draw(const Sprite& sprite, const Rectangle& dst, Color32 color)
{
	instance.GetDrawCommand(sprite.textureId).count += 6;

	Vector2Int size = dst.size;

	Vector2 start = sprite.uv[0];
	Vector2 end = sprite.uv[1];

	instance.AddVertex(Vector2(dst.position), start, color);
	instance.AddVertex(Vector2(dst.position + Vector2Int(size.x, 0)), { end.x,start.y }, color);
	instance.AddVertex(Vector2(dst.position + size), end, color);

	instance.AddVertex(Vector2(dst.position + size), end, color);
	instance.AddVertex(Vector2(dst.position + Vector2Int(0, size.y)), { start.x,end.y }, color);
	instance.AddVertex(Vector2(dst.position), start, color);
}

void GraphicsRenderer::DrawText(const Font& font, Vector2Int position, const char* text, Color color, float scale)
{
	Submit();

	Platform::DrawText(font, position, text, color, scale);
}

void GraphicsRenderer::DrawRectangle(const Rectangle& dst, Color32 color)
{
	instance.GetDrawCommand(DrawCommandType::Triangle).count += 6;

	instance.AddVertex(Vector2(dst.position), { 0,0 }, color);
	instance.AddVertex(Vector2(dst.position + Vector2Int(dst.size.x, 0)), { 1,0, }, color);
	instance.AddVertex(Vector2(dst.position + dst.size), { 1,1 }, color);

	instance.AddVertex(Vector2(dst.position + dst.size), { 1,1 }, color);
	instance.AddVertex(Vector2(dst.position + Vector2Int(0, dst.size.y)), { 0,1 }, color);
	instance.AddVertex(Vector2(dst.position), { 0,0 }, color);
}

void GraphicsRenderer::DrawLine(Vector2Int src, Vector2Int dst, Color32 color)
{
	instance.GetDrawCommand(DrawCommandType::Line).count += 2;

	instance.AddVertex(Vector2(src), { 0,0 }, color);
	instance.AddVertex(Vector2(dst), { 1,1, }, color);
}

void GraphicsRenderer::Submit()
{
	if (!instance.drawCommands.size())
		return;

	Platform::ExecDrawCommands({ instance.drawCommands.data(), instance.drawCommands.size() });

	instance.texture = nullptr;
	instance.cmdType = DrawCommandType::None;
	instance.drawCommands.clear();
	instance.vbPos = 0;
}

void GraphicsRenderer::DrawOnScreen(ScreenId screen)
{
	Submit();

	Platform::DrawOnScreen(screen);
}

void GraphicsRenderer::DrawOnSurface(RenderSurface surface)
{
	Submit();
	Platform::DrawOnSurface(surface.surfaceId);
}

void GraphicsRenderer::DrawOnCurrentScreen()
{
	Submit();
	Platform::DrawOnSurface(nullptr);
}

RenderSurface GraphicsRenderer::NewRenderSurface(Vector2Int size, bool pixelFiltering)
{
	return Platform::NewRenderSurface(size, pixelFiltering);
}

void GraphicsRenderer::ChangeBlendingMode(BlendMode mode)
{
	if (mode == instance.blendMode)
		return;

	Submit();

	instance.blendMode = mode;

	Platform::ChangeBlendingMode(instance.blendMode);
}

void GraphicsRenderer::ClearCurrentSurface(Color color)
{
	Platform::ClearBuffer(color);
}

Sprite GraphicsRenderer::NewSprite(Texture texture, const Rectangle16& rect)
{
	return  Platform::NewSprite({ texture }, rect);
}
