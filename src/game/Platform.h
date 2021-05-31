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

	// ================ General Purpose =================

	static Vector2Int MeasureString(Font font, const char* text, float scale = 1);
	static double ElaspedTime();

	// ================ File System ======================
	
	static const SpriteAtlas* LoadAtlas(const char* path);
	static Font LoadFont(const char* path);
	static FILE* OpenAsset(const char* path);
	static std::string GetUserDirectory();

	// =============== Graphics =========================

	static void DrawOnScreen(ScreenId screen);
	static void DrawOnSurface(Surface surface);
	static void ChangeBlendingMode(BlendMode mode);
	static Sprite NewSprite(Texture texture, Rectangle16 src);
	static void ClearBuffer(Color color);
	static Span<Vertex> GetVertexBuffer();
	static void ExecDrawCommands(const Span<DrawCommand> commands);
	static void DrawText(const Font& font, Vector2Int position, const char* text, Color color= Colors::White, float scale = 1.0f);
	static RenderSurface NewRenderSurface(Vector2Int size, bool pixelFiltering = false);

	// ================ Input =======================

	static void UpdateGamepadState(GamepadState& state);
	static void UpdatePointerState(PointerState& state);

	// ================ Audio =======================

	static void CreateChannel(AudioChannelState& channel);
	static void EnableChannel(const AudioChannelState& channel, bool enabled);

	// ================= Threading ==================

	static int StartThreads(std::function<void(int)> threadWork);
	static Semaphore CreateSemaphore();
	static void WaitSemaphore(Semaphore);
	static void ReleaseSemaphore(Semaphore, int);

};




