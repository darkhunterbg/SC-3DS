#pragma once

#include "Assets.h"
#include "MathLib.h"
#include "Color.h"

#include "Engine/InputDevices.h"
#include "Engine/GraphicsPrimitives.h"
#include "Engine/AudioChannel.h"

#include <functional>


typedef void* Semaphore;


class Platform {

private:
	Platform() = delete;
	~Platform() = delete;

public:

	// ================ General Purpose =================

	static Vector2Int MeasureString(const Font& font, const char* text);
	static double ElaspedTime();

	// ================ File System ======================
	
	static TextureId LoadTexture(const char* path, Vector2Int16& outSize);
	static const Font* LoadFont(const char* path, int size);
	static FILE* OpenAsset(const char* path);
	static std::string GetUserDirectory();

	// =============== Graphics =========================

	static SubImageCoord GenerateUV(TextureId texture, Rectangle16 src);
	static void DrawOnScreen(ScreenId screen);
	static void DrawOnSurface(SurfaceId surface);
	static void ChangeBlendingMode(BlendMode mode);
	static void ClearBuffer(Color color);

	static void DrawTexture(const Texture& texture, const SubImageCoord& uv, const Rectangle16& dst, bool hFlip = false, Color32 color = Color32(Colors::White));
	static void DrawRectangle(const Rectangle& rect, Color32 color = Color32(Colors::White));
	static void DrawText(const Font& font, Vector2Int position, const char* text, Color color = Colors::White);
	static SurfaceId NewRenderSurface(Vector2Int size, bool pixelFiltering, TextureId& outTexture);

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




