#pragma once

#include "../Assets.h"
#include "../Color.h"
#include "../MathLib.h"
#include "../Engine/GraphicsPrimitives.h"
#include <vector>
#include <unordered_map>
#include <memory>

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

class GUIState {
public:
	struct Resource {
		void* memory;
		void (*freeFunc)(void*);
	};

	std::vector<Rectangle> SpaceStack;
	std::unordered_map<std::string, Resource> Resources;
	double VideoPlaybackCooldown = 16.6;

	Rectangle GetSpace()const { return SpaceStack.back(); }
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

	static Vector2Int GetPosition(Vector2Int pos);
	static Vector2Int GetRelativePosition(Vector2Int pos,
		GUIHAlign hAlign = GUIHAlign::Center,
		GUIVAlign vAlign = GUIVAlign::Center);

	static Vector2Int GetRelativePosition(Rectangle rect,
		GUIHAlign hAlign = GUIHAlign::Center,
		GUIVAlign vAlign = GUIVAlign::Center);

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

	static bool IsLayoutHover();

	static void SetVideoPlaybackSpeed(double speed)
	{
		GetState().VideoPlaybackCooldown = 16.6 * speed;
	}
};

