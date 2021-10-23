#pragma once

#include "Assets.h"
#include "MathLib.h"
#include "Color.h"

#include "Engine/InputDevices.h"
#include "Engine/GraphicsPrimitives.h"
#include "Engine/AudioChannel.h"

#include <functional>


typedef void* Semaphore;

enum class PlatformType {
	Generic = 0,
	Nintendo3DS = 1,
};

enum class ThreadUsageType {
	JobSystem = 0,
	IO = 1,
};

struct PlatformInfo {
public:
	std::vector<Vector2Int16> Screens;
	bool PointerIsCursor = false;
	PlatformType Type = PlatformType::Generic;
	int HardwareThreadsCount = 1;
};


class Platform {

private:
	Platform() = delete;
	~Platform() = delete;

public:

	// ================ General Purpose =================

	static PlatformInfo GetPlatformInfo();
	static Vector2Int MeasureString(const Font& font, const char* text);
	static double ElaspedTime();

	// ================ File System ======================

	static TextureId CreateTextureFromFile(const char* path, Span<uint8_t> data, Vector2Int16& outSize);
	static const Font* LoadFont(const char* path, int size);
	static FILE* OpenAsset(const char* path, AssetType type);
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
	static TextureId NewTexture(Vector2Int size, bool pixelFiltering);
	static void UpdateTexture(TextureId texture, Rectangle part, Span<uint8_t> bytes);
	static void DestroyTexture(TextureId texture);
	// ================ Input =======================

	static void UpdateGamepadState(GamepadState& state);
	static void UpdatePointerState(PointerState& state);

	// ================ Audio =======================

	static void CreateChannel(AudioChannelState& channel);
	static void EnableChannel(const AudioChannelState& channel, bool enabled);

	// ================= Threading ==================

	static int TryStartThreads(ThreadUsageType type, int requestCount, std::function<void(int)> threadWork);
	static Semaphore CreateSemaphore();
	static void WaitSemaphore(Semaphore);
	static void ReleaseSemaphore(Semaphore, int);

	// ================ Other ========================

	static void* PlatformAlloc(unsigned size);
	static void PlatformFree(void* ptr);
};




