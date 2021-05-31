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
	static AudioClip ButtonAudio;

	static Gamepad Gamepad;
	static Pointer Pointer;

	static float DeltaTime;

	static void Start();
	static void FrameStart();
	static void FrameEnd();
	static bool Update();
	static void Draw();
	static void End();

	static void SetCurrentScene(Scene* scene);
};