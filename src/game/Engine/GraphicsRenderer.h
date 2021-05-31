#pragma once


#include "../Platform.h"
#include "GraphicsPrimitives.h"

#include <vector>

#include <Span.h>

class GraphicsRenderer {

private:

	std::vector<DrawCommand> drawCommands;
	Span<Vertex> vertexBuffer;
	unsigned vbPos = 0;
	Texture texture = nullptr;
	DrawCommandType cmdType = DrawCommandType::None;
	BlendMode blendMode = BlendMode::Alpha;

	void AddVertex(const Vector2& pos, const Vector2& uv, const Color32& c);

	DrawCommand& NewDrawCommand(Texture texture);
	DrawCommand& GetDrawCommand(Texture texture);
	DrawCommand& GetDrawCommand(DrawCommandType type);

	GraphicsRenderer() {}

	GraphicsRenderer(const GraphicsRenderer&) = delete;
	GraphicsRenderer& operator=(const GraphicsRenderer&) = delete;
	static GraphicsRenderer instance;
public:
	static void Init();
	static void BufferDraw(std::vector<BatchDrawCommand>& cmd);
	static void Draw(const Sprite& sprite, Vector2Int position, Color32 color= 0xFFFFFFFF);
	static void Draw(const Sprite& sprite, const Rectangle& dst, Color32 color = 0xFFFFFFFF);

	inline static void Draw(const Sprite& sprite, Vector2Int position, Color color) {
		Draw(sprite, position, Color32(color));
	}
	inline static void Draw(const Sprite& sprite, const Rectangle& dst, Color color) {
		Draw(sprite, dst, Color32(color));
	}

	static void DrawText(const Font& font, Vector2Int position, const char* text, Color color = Colors::White, float scale = 1.0f);

	static void DrawRectangle(const Rectangle& rect,  Color32 color);

	inline static void DrawRectangle(const Rectangle& rect, const Color& color) {
		DrawRectangle(rect, Color32(color));
	}

	static void Submit();

	static void DrawOnScreen(ScreenId screen);

	static void DrawOnTexture(Texture texture);

	static void ChangeBlendingMode(BlendMode mode);
};