#include "GraphicsRenderer.h"

#include "../Platform.h"
#include "../Debug.h"


GraphicsRenderer GraphicsRenderer::instance;

static inline void SwapUV(Vector2& a, Vector2& b)
{
	Vector2 temp = { a.x, a.y };
	a.x = b.x;
	a.y = b.y;
	b.x = temp.x;
	b.y = temp.y;
}


void GraphicsRenderer::Init()
{
	instance.vertexBuffer = Platform::GetVertexBuffer();
	instance.vb = instance.vertexBuffer.Data();
	Platform::ChangeBlendingMode(instance.blendMode);
}


void GraphicsRenderer::AddVertex(const Vector2& pos, const Vector2& uv, const Color32& c)
{
	if (vbPos == vertexBuffer.Size())
		EXCEPTION("VertexBuffer overflow!");

	auto& v = vb[vbPos++];
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

DrawCommand& GraphicsRenderer::NewDrawCommand(const Texture* texture)
{
	instance.drawCommands.push_back({ texture , (uint16_t)instance.vbPos,0 ,DrawCommandType::TexturedTriangle });
	instance.texture = texture;
	instance.cmdType = DrawCommandType::TexturedTriangle;

	return instance.drawCommands.back();
}

DrawCommand& GraphicsRenderer::GetDrawCommand(const Texture* texture)
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
	if (cmd.size() == 0)
		return;

	if (instance.drawCommands.size() == 0)
		instance.drawCommands.push_back({ cmd.front().sprite->texture , (uint16_t)instance.vbPos,0 , DrawCommandType::TexturedTriangle });

	DrawCommand* dc = &instance.drawCommands.front();

	for (const auto& c : cmd) {

		Vector2Int16 size = c.sprite->size;
		size.x *= c.scale.x;
		size.y *= c.scale.y;


		Vector2 tL = c.sprite->uv.coords[0];
		Vector2 tR = c.sprite->uv.coords[1];
		Vector2 bL = c.sprite->uv.coords[2];
		Vector2 bR = c.sprite->uv.coords[3];

		if (size.x < 0) {
			size.x = -size.x;
			SwapUV(tL, tR);
			SwapUV(bL, bR);
		}


		if (dc->texture != c.sprite->texture) {
			dc = &instance.NewDrawCommand(c.sprite->texture);
		}


		instance.AddVertex(Vector2(c.position), tL, c.color);
		instance.AddVertex(Vector2(c.position + Vector2Int16(size.x, 0)), tR, c.color);
		instance.AddVertex(Vector2(c.position + size), bR, c.color);

		instance.AddVertex(Vector2(c.position + size), bR, c.color);
		instance.AddVertex(Vector2(c.position + Vector2Int16(0, size.y)), bL, c.color);
		instance.AddVertex(Vector2(c.position), tL, c.color);

		dc->count += 6;
	}

	instance.texture = instance.drawCommands.front().texture;

}

void GraphicsRenderer::Draw(const Texture& texture, Vector2Int position, Vector2Int size, Color32 color)
{
	instance.GetDrawCommand(&texture).count += 6;


	auto uv = Platform::GenerateUV(texture.GetTextureId(), { {0,0}, Vector2Int16(size) });

	instance.AddVertex(Vector2(position), uv.corners.topLeft, color);
	instance.AddVertex(Vector2(position + Vector2Int(size.x, 0)), uv.corners.topRight, color);
	instance.AddVertex(Vector2(position + size), uv.corners.bottomRight, color);

	instance.AddVertex(Vector2(position + size), uv.corners.bottomRight, color);
	instance.AddVertex(Vector2(position + Vector2Int(0, size.y)), uv.corners.bottomLeft, color);
	instance.AddVertex(Vector2(position), uv.corners.topLeft, color);
}

void GraphicsRenderer::Draw(const ImageFrame& sprite, Vector2Int position, Color32 color)
{
	if (sprite.texture == nullptr)
		return;

	position += Vector2Int(sprite.offset);

	instance.GetDrawCommand(sprite.texture).count += 6;

	Vector2Int size = Vector2Int(sprite.size);

	const Vector2& tL = sprite.uv.coords[0];
	const Vector2& tR = sprite.uv.coords[1];
	const Vector2& bL = sprite.uv.coords[2];
	const Vector2& bR = sprite.uv.coords[3];

	instance.AddVertex(Vector2(position), tL, color);
	instance.AddVertex(Vector2(position + Vector2Int(size.x, 0)), tR, color);
	instance.AddVertex(Vector2(position + size), bR, color);

	instance.AddVertex(Vector2(position + size), bR, color);
	instance.AddVertex(Vector2(position + Vector2Int(0, size.y)), bL, color);
	instance.AddVertex(Vector2(position), tL, color);
}

void GraphicsRenderer::Draw(const ImageFrame& sprite, const Rectangle& dst, Color32 color)
{
	if (sprite.texture == nullptr)
		return;

	instance.GetDrawCommand(sprite.texture).count += 6;

	Vector2Int size = dst.size;

	const Vector2& tL = sprite.uv.coords[0];
	const Vector2& tR = sprite.uv.coords[1];
	const Vector2& bL = sprite.uv.coords[2];
	const Vector2& bR = sprite.uv.coords[3];

	instance.AddVertex(Vector2(dst.position), tL, color);
	instance.AddVertex(Vector2(dst.position + Vector2Int(size.x, 0)), tR, color);
	instance.AddVertex(Vector2(dst.position + size), bR, color);

	instance.AddVertex(Vector2(dst.position + size), bR, color);
	instance.AddVertex(Vector2(dst.position + Vector2Int(0, size.y)), bL, color);
	instance.AddVertex(Vector2(dst.position), tL, color);
}

void GraphicsRenderer::DrawText(const Font& font, Vector2Int position, const char* text, Color color)
{
	Submit();

	Platform::DrawText(font, position, text, color);
}

void GraphicsRenderer::DrawRectangle(const Rectangle& dst, Color32 color)
{
	//return;
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
	instance.GetDrawCommand(DrawCommandType::Triangle).count += 6;

	Vector2 dir = Vector2(dst - src);
	dir.Normalize();

	float f = std::round(dir.x);
	dir.x = std::round(dir.y);
	dir.y = f;

	instance.AddVertex(Vector2(src), { 0,0 }, color);
	instance.AddVertex(Vector2(dst.x, src.y) + Vector2{ dir.x, 0 }, { 1,0, }, color);
	instance.AddVertex(Vector2(dst) + dir, { 1, 1 }, color);

	instance.AddVertex(Vector2(dst) + dir, { 1,1 }, color);
	instance.AddVertex(Vector2(src.x, dst.y) + Vector2(0, dir.y), { 0,1 }, color);
	instance.AddVertex(Vector2(src), { 0,0 }, color);
}

void GraphicsRenderer::NewFrame()
{
	instance.vbPos = 0;
}

void GraphicsRenderer::EndFrame()
{
	Submit();
	instance.vbPos = 0;
}

void GraphicsRenderer::Submit()
{
	if (!instance.drawCommands.size())
		return;

	Platform::ExecDrawCommands({ instance.drawCommands.data(), instance.drawCommands.size() });

	instance.texture = nullptr;
	instance.cmdType = DrawCommandType::None;
	instance.drawCommands.clear();
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
	TextureId t;
	SurfaceId  s = Platform::NewRenderSurface(size, pixelFiltering, t);
	RenderSurface rs;

	rs.surfaceId = s;
	rs.sprite.size = Vector2Int16(size);
	rs.sprite.texture = new Texture("RenderTarget", Vector2Int16(size), t);
	rs.sprite.uv = Platform::GenerateUV(t, { {0,0}, Vector2Int16(size) });

	return rs;
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

ImageFrame GraphicsRenderer::NewSprite(const Texture& texture, const Rectangle16& rect)
{
	ImageFrame f;
	f.texture = &texture;
	f.offset = rect.position;
	f.size = rect.GetSize();
	f.uv = Platform::GenerateUV(texture.GetTextureId(), rect);

	return f;
}
