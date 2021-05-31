#pragma once

#include "Assets.h"

class Scene;

class Game {

private:
	Game() = delete;
	~Game() = delete;

public:
	static const Font* SystemFont12;
	static const Font* SystemFont10;
	static const Font* SystemFont8;
	static AudioClip ButtonAudio;

	static float DeltaTime;

	static void Start();
	static void FrameStart();
	static void FrameEnd();
	static bool Update();
	static void Draw();
	static void End();

	static void SetCurrentScene(Scene* scene);
};