#pragma once

#include "../Assets.h"
#include "../Color.h"
#include "../MathLib.h"
#include "../Engine/GraphicsPrimitives.h"
#include <vector>
#include <unordered_map>
#include <memory>
#include <bitset>
#include <functional>

enum class GUIHAlign {
	Left = 0,
	Center = 1,
	Right = 2,
};

enum class GUIVAlign {
	Top = 0,
	Center = 1,
	Bottom = 2,
};

enum  GUITextEffects : uint8_t {
	GUITextEffects_None = 0,
	GUITextEffects_Shadow = 1,
};



class GUIState {
public:
	struct Resource {
		void* memory;
		void (*freeFunc)(void*);
	};

	std::vector<Rectangle> SpaceStack;
	std::unordered_map<std::string, Resource> Resources;
	std::vector < std::function<void()>> ExecAtEndOfFrame;

	double VideoPlaybackTickTime = 16.67;
	int ImageAnimationTimer = 6;

	Vector2Int ElementOffset = { 0,0 };
	Vector2Int ElementSize = { 0, 0 };


	bool Focused = false;
	bool NextFocused = false;

	Rectangle GetSpace()const { return SpaceStack.back(); }

	Vector2Int PopOffset() { Vector2Int o = ElementOffset; ElementOffset = { 0,0 }; return o; }
	void OverrideSize(Vector2Int& size)
	{
		if (ElementSize.LengthSquared() > 0)
		{
			size = ElementSize;
			ElementSize = { 0,0 };
		}
	}

	std::bitset<32> TextEffects = { 0 };

	bool RestartVideo = false;
};

class GUI {
private:
	GUI() = delete;
	~GUI() = delete;

	static GUIState* _state;
public:
	static Vector2Int GetScreenSize();

	static void SetState(GUIState& state) { _state = &state; }
	static GUIState& GetState() { return *_state; }

	static void UseScreen(ScreenId screen);

	static void BeginAbsoluteLayout(Vector2Int position, Vector2Int size) { BeginAbsoluteLayout({ position,size }); }
	static void BeginAbsoluteLayout(Rectangle layout);
	static void BeginRelativeLayout(Vector2Int position, Vector2Int size,
		GUIHAlign hAlign = GUIHAlign::Center,
		GUIVAlign vAlign = GUIVAlign::Center)
	{
		BeginRelativeLayout({ position,size }, hAlign, vAlign);
	}
	static void BeginRelativeLayout(Rectangle layout,
		GUIHAlign hAlign = GUIHAlign::Center,
		GUIVAlign vAlign = GUIVAlign::Center);

	static void EndLayout();

	static Vector2Int GetPosition(Vector2Int pos,
		GUIHAlign hAlign = GUIHAlign::Left,
		GUIVAlign vAlign = GUIVAlign::Top);

	static Vector2Int GetPosition(Rectangle rect,
		GUIHAlign hAlign = GUIHAlign::Left,
		GUIVAlign vAlign = GUIVAlign::Top);

	static Rectangle GetLayoutSpace();

	template <class TResource>
	static TResource* GetResourceById(const std::string& key)
	{
		auto state = GetState();
		auto result = state.Resources.find(key);
		if (result == state.Resources.end()) return nullptr;
		return static_cast<TResource*>(result->second.memory);
	}
	static void RegisterResource(const std::string& key, void* resource, void (*freeFunc)(void*));

	static void CleanResources();
	static void FrameEnd();

	static bool IsLayoutFocused();
	static bool IsLayoutPressed();

	static bool OnLayoutActivated();
	static bool OnLayoutActivatedAlt();
	static bool OnLayoutFocused();

	static void SetVideoPlaybackSpeed(double speed)
	{
		GetState().VideoPlaybackTickTime = 16.67* speed;
	}

	static void AddNextElementOffset(Vector2Int offset)
	{
		GetState().ElementOffset += offset;
	}
	static void SetNextElementSize(Vector2Int size)
	{
		GetState().ElementSize = size;
	}

	static Vector2Int GetMousePosition();
	static Vector2Int GetPointerPosition();

	static void DrawLayoutDebug();

	static void DrawAtEndOfFrame(std::function<void()> func)
	{
		_state->ExecAtEndOfFrame.push_back(func);
	}

};

