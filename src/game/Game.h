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

	static void Start();
	static bool Update();
	static void Draw();
	static void End();


	static void SetCurrentScene(Scene* scene);
};