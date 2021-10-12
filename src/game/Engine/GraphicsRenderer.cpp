#include "GraphicsRenderer.h"

#include "../Platform.h"
#include "../Debug.h"


GraphicsRenderer GraphicsRenderer::instance;

void GraphicsRenderer::Init()
{
	Platform::ChangeBlendingMode(instance.blendMode);
}

void GraphicsRenderer::BufferDraw(std::vector<BatchDrawCommand>& cmd)
{
	if (cmd.size() == 0)
		return;

	for (const auto& c : cmd)
	{

		Rectangle16 dst = { c.position, c.sprite->size };
		dst.size = Vector2Int16(Vector2(dst.size) * c.scale);
		bool hFlips = c.scale.x < 0.1f;

		if (hFlips)
			dst.size.x *= -1;

		Platform::DrawTexture(*c.sprite->texture, c.sprite->uv, dst, hFlips, c.color.value);
	}
}

void GraphicsRenderer::Draw(const Texture& texture, Vector2Int position, Vector2Int size, Color32 color)
{
	Rectangle16 dst = { Vector2Int16(position), Vector2Int16(size) };
	Rectangle16 src = { Vector2Int16(0,0), texture.GetSize() };
	Platform::DrawTexture(texture, src, dst, false, color.value);
}

void GraphicsRenderer::Draw(const ImageFrame& sprite, Vector2Int position, Color32 color)
{
	if (sprite.texture == nullptr)
		return;

	Rectangle16 rect = { Vector2Int16(position), sprite.size };

	Platform::DrawTexture(*sprite.texture, sprite.uv, rect, false, color.value);
}

void GraphicsRenderer::Draw(const ImageFrame& sprite, const Rectangle& dst, Color32 color)
{
	if (sprite.texture == nullptr)
		return;

	Rectangle16 rect = { Vector2Int16(dst.position), Vector2Int16(dst.size) };

	Platform::DrawTexture(*sprite.texture, sprite.uv, rect, false, color.value);
}

void GraphicsRenderer::DrawText(const Font& font, Vector2Int position, const char* text, Color color)
{
	Submit();

	Platform::DrawText(font, position, text, color);
}

void GraphicsRenderer::DrawRectangle(const Rectangle& dst, Color32 color)
{
	Platform::DrawRectangle(dst, color.value);
}

void GraphicsRenderer::DrawLine(Vector2Int src, Vector2Int dst, Color32 color)
{
	Rectangle rect;
	rect.position = Vector2Int(src);
	if (dst.x < rect.position.x)
		rect.position.x = dst.x;

	if (dst.y < rect.position.y)
		rect.position.y = dst.y;

	rect.size = (dst - src);
	rect.size.x = fabs(rect.size.x);
	rect.size.y = fabs(rect.size.y);

	if (rect.size.x == 0)
		rect.size.x = 1;
	if (rect.size.y == 0)
		rect.size.y = 1;

	Platform::DrawRectangle(rect, color);
}

void GraphicsRenderer::NewFrame()
{
}

void GraphicsRenderer::EndFrame()
{
	Submit();

}

void GraphicsRenderer::Submit()
{
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
