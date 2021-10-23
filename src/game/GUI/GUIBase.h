#pragma once

#include "../Assets.h"
#include "../Color.h"
#include "../MathLib.h"
#include "../Engine/GraphicsPrimitives.h"
#include <vector>
#include <unordered_map>
#include <memory>

enum class GUIHorizontalAlignment {
	Left = 0,
	Center = 1,
	Right = 2,
};

enum class GUIVerticalAlignment {
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

	static void BeginAbsoluteLayout(Rectangle layout);
	static void BeginRelativeLayout(Rectangle layout,
		GUIHorizontalAlignment hAlign = GUIHorizontalAlignment::Center,
		GUIVerticalAlignment vAlign = GUIVerticalAlignment::Center);

	static void EndLayout();

	static Vector2Int GetPosition(Vector2Int pos);
	static Vector2Int GetRelativePosition(Vector2Int pos,
		GUIHorizontalAlignment hAlign = GUIHorizontalAlignment::Center,
		GUIVerticalAlignment vAlign = GUIVerticalAlignment::Center);

	static Vector2Int GetRelativePosition(Rectangle rect,
		GUIHorizontalAlignment hAlign = GUIHorizontalAlignment::Center,
		GUIVerticalAlignment vAlign = GUIVerticalAlignment::Center);

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
};

