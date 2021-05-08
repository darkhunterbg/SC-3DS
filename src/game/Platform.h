#pragma once

#include "Assets.h"
#include "MathLib.h"
#include "Color.h"
#include "Input.h"
#include <functional>


struct AudioChannelState;
typedef int AudioChannelHandle;
typedef void* Semaphore;

struct DrawCommandColor {
	Color4 color;
	float blend;

	inline void AlphaBlend(float alpha) {
		color = Color4(0.0f, 0.0f, 0.0f, alpha);
		blend = 0;
	}
};

struct BatchDrawCommand {
	int order;
	Image image;
	Vector2Int16 position;
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

	static void BatchDraw(Span< BatchDrawCommand> commands);
	static void Draw(const Sprite& sprite, Rectangle dst, Color additiveBlendColor = Colors::Black, bool hFlip = false);
	static void DrawText(const Font& font, Vector2Int position, const char* text, Color color, float scale = 1.0f);
	static void DrawLine(Vector2Int src, Vector2Int dst, Color color);
	static void DrawRectangle(Rectangle rect, Color color);
	static Image NewTexture(Vector2Int size);
	static FILE* OpenAsset(const char* path);

	static double ElaspedTime();

	static void UpdateGamepadState(GamepadState& state);
	static void UpdatePointerState(PointerState& state);

	static void CreateChannel(AudioChannelState& channel);
	static void EnableChannel(const AudioChannelState& channel, bool enabled);

	static AudioClip LoadAudioClip(const char* path);
	static AudioStream* LoadAudioStream(const char* path);

	static int StartThreads(std::function<void(int)> threadWork);
	static Semaphore CreateSemaphore();
	static void WaitSemaphore(Semaphore);
	static void ReleaseSemaphore(Semaphore, int);
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


