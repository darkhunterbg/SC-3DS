#pragma once

#include "Assets.h"
#include "Input.h"

class Scene;

class Game {

private:
	Game() = delete;
	~Game() = delete;

public:
	static Font SystemFont;
	static GamepadState Gamepad;
	static PointerState Pointer;

	static float FrameTime;

	static void Start();
	static void FrameStart();
	static bool Update();
	static void Draw();
	static void End();


	static void SetCurrentScene(Scene* scene);
};