#pragma once

#include "Assets.h"
#include "Audio.h"
#include "AssetLoader.h"
#include "Input.h"
#include "Engine/GraphicsRenderer.h"

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
	static AudioSystem Audio;
	static AssetLoader AssetLoader;
	static GraphicsRenderer Renderer;

	static float DeltaTime;

	static void Start();
	static void FrameStart();
	static void FrameEnd();
	static bool Update();
	static void Draw();
	static void End();

	static void SetCurrentScene(Scene* scene);
};