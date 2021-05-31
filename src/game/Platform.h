#pragma once

#include "Assets.h"
#include "MathLib.h"
#include "Color.h"
#include "Input.h"
#include "Engine/GraphicsPrimitives.h"


#include <functional>


struct AudioChannelState;
typedef int AudioChannelHandle;
typedef void* Semaphore;



class Platform {

private:
	Platform() = delete;
	~Platform() = delete;

public:
	static const SpriteAtlas* LoadAtlas(const char* path);
	static Font LoadFont(const char* path);
	static Vector2Int MeasureString(Font font, const char* text, float scale = 1);
	static void DrawOnScreen(ScreenId screen);
	static void DrawOnTexture(Texture texture);

	static void ChangeBlendingMode(BlendMode mode);
	//static void ToggleTestBlend();

	static Sprite NewSprite(Image image, Rectangle16 src);
	static void ClearBuffer(Color color);


	static Span<Vertex> GetVertexBuffer();
	static void ExecDrawCommands(const Span<DrawCommand> commands);

	static void DrawText(const Font& font, Vector2Int position, const char* text, Color color= Colors::White, float scale = 1.0f);
	static void DrawLine(Vector2Int src, Vector2Int dst, Color color);
	static Image NewTexture(Vector2Int size, bool pixelFiltering = false);
	static FILE* OpenAsset(const char* path);

	static double ElaspedTime();

	static void UpdateGamepadState(GamepadState& state);
	static void UpdatePointerState(PointerState& state);

	static void CreateChannel(AudioChannelState& channel);
	static void EnableChannel(const AudioChannelState& channel, bool enabled);

	static int StartThreads(std::function<void(int)> threadWork);
	static Semaphore CreateSemaphore();
	static void WaitSemaphore(Semaphore);
	static void ReleaseSemaphore(Semaphore, int);


	static std::string GetUserDirectory();
};




