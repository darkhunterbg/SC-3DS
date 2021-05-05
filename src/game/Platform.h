#pragma once

#include "Assets.h"
#include "MathLib.h"
#include "Color.h"
#include "Input.h"


struct AudioChannelState;
typedef int AudioChannelHandle;

struct DrawCommandColor {
	uint32_t color;
	float blend;

	constexpr  uint32_t Color32(char r, char g, char b, char a)
	{
		return r | (g << (uint32_t)8) | (b << (uint32_t)16) | (a << (uint32_t)24);
	}

	inline void AlphaBlend(float alpha) {
		color = { Color32(0,0,0, alpha * 255) };
		blend = 0;
	}
};

struct BatchDrawCommand {
	Image image;
	Vector2 position;
	Vector2 scale;
	DrawCommandColor color;
};


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

	static void TestDraw(Image sprite, Vector2Int  pos, float scale = 1, bool hFlip = false);

	static void BatchDraw(Span< BatchDrawCommand> commands);
	static void Draw(const Sprite& sprite, Rectangle dst, Color additiveBlendColor = Colors::Black, bool hFlip = false);
	static void DrawText(const Font& font, Vector2Int position, const char* text, Color color, float scale = 1.0f);
	static void DrawLine(Vector2Int src, Vector2Int dst, Color color);
	static void DrawRectangle(Rectangle rect, Color color);
	static Texture NewTexture(Vector2Int size);
	static FILE* OpenAsset(const char* path);

	static double ElaspedTime();

	static void UpdateGamepadState(GamepadState& state);
	static void UpdatePointerState(PointerState& state);

	static void CreateChannel(AudioChannelState& channel);
	static void EnableChannel(const AudioChannelState& channel, bool enabled);

	static AudioClip LoadAudioClip(const char* path);
	static AudioStream* LoadAudioStream(const char* path);
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


