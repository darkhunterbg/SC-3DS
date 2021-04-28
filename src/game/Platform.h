#pragma once

#include "Assets.h"
#include "MathLib.h"
#include "Color.h"
#include "Input.h"



enum class ScreenId
{
	Top = 0,
	Bottom = 1,
};


class Platform {

private:
	Platform() = delete;
	~Platform() = delete;

public:
	static const SpriteAtlas* LoadAtlas(const char* path);
	static Font LoadFont(const char* path);
	static void DrawOnScreen(ScreenId screen);
	static void DrawOnTexture(Texture texture);
	static void Draw(const Sprite& sprite, Rectangle dst, Color color);
	static void DrawText(const Font& font, Vector2Int position, const char* text, Color color, float scale = 1.0f);
	static void DrawLine(Vector2Int src, Vector2Int dst, Color color);
	static void DrawRectangle(Rectangle rect, Color color);
	static Texture NewTexture(Vector2Int size);

	static double ElaspedTime();

	static void UpdateGamepadState(GamepadState& state);
	static void UpdatePointerState(PointerState& state);
};


#if defined (_3DS)
extern void FatalError(const char* error, ...);
#define EXCEPTION(TEXT, ...)  FatalError(TEXT, ## __VA_ARGS__);

#elif defined(_WIN32)
#if defined(_DEBUG)
#define EXCEPTION(TEXT, ...) __debugbreak();  /// ## __VA_ARGS__
#else 
#define EXCEPTION(TEXT, ...) 
#endif
#endif


