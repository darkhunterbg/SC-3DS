#pragma once

#include "GraphicsPrimitives.h"

#include <vector>
#include <Span.h>

#include "../Assets.h"

class GraphicsRenderer {

private:
	BlendMode blendMode = BlendMode::Alpha;

	GraphicsRenderer() {}

	GraphicsRenderer(const GraphicsRenderer&) = delete;
	GraphicsRenderer& operator=(const GraphicsRenderer&) = delete;
	static GraphicsRenderer instance;
public:
	static void Init();
	static void BufferDraw(std::vector<BatchDrawCommand>& cmd);

	static void Draw(const Texture& texture, Vector2Int position, Vector2Int size, Color32 color = 0xFFFFFFFF);

	static void Draw(const ImageFrame& sprite, Vector2Int position, Color32 color = 0xFFFFFFFF);
	static void Draw(const ImageFrame& sprite, const Rectangle& dst, Color32 color = 0xFFFFFFFF);

	inline static void Draw(const RenderSurface& surface, Vector2Int position, Color32 color = 0xFFFFFFFF) {
		Draw(surface.sprite, position, color);
	}

	inline static void Draw(const RenderSurface& surface, const Rectangle& dst,  Color32 color = 0xFFFFFFFF) {
		Draw(surface.sprite, dst, color);
	}

	inline static void Draw(const ImageFrame& sprite, Vector2Int position , Color color ) {
		Draw(sprite, position, Color32(color));
	}
	inline static void Draw(const ImageFrame& sprite, const Rectangle& dst, Color color ) {
		Draw(sprite, dst, Color32(color));
	}
	inline static void Draw(const RenderSurface& surface, Vector2Int position, Color color ) {
		Draw(surface.sprite, position, Color32(color));
	}
	inline static void Draw(const RenderSurface& surface, const Rectangle& dst, Color color) {
		Draw(surface.sprite, dst, Color32(color));
	}

	static void DrawText(const Font& font, Vector2Int position, const char* text, Color color = Colors::White);

	static void DrawRectangle(const Rectangle& rect, Color32 color);

	inline static void DrawRectangle(const Rectangle& rect, const Color& color) {
		DrawRectangle(rect, Color32(color));
	}
	static void DrawLine(Vector2Int src, Vector2Int dst, Color32 color);

	inline static void DrawLine(Vector2Int src, Vector2Int dst, Color color) {
		DrawLine(src, dst ,Color32(color));
	}

	static void NewFrame();

	static void EndFrame();

	static void Submit();

	static void DrawOnScreen(ScreenId screen);

	static void DrawOnSurface(RenderSurface surface);

	static void DrawOnCurrentScreen();

	static RenderSurface NewRenderSurface(Vector2Int size, bool pixelFiltering = false);

	static void ChangeBlendingMode(BlendMode mode);

	static void ClearCurrentSurface(Color color);

	static ImageFrame NewSprite(const Texture& texture, const Rectangle16& rect);

	static Vector2Int16 GetScreenSize(ScreenId screen);
};