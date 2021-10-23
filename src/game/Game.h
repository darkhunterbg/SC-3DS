#pragma once

#include "Assets.h"

class Scene;
struct PlatformInfo;

struct GameStartSettings {
	bool skipIntro = false;
	bool mute = false;
};

class Game {

private:
	Game() = delete;
	~Game() = delete;
public:
	static const Font* SystemFont16;
	static const Font* SystemFont12;
	static const Font* SystemFont10;
	static const Font* SystemFont8;
	static AudioClip* ButtonAudio;

	static float DeltaTime;

	static void Start(GameStartSettings settings = {});
	static void FrameStart();
	static void FrameEnd();
	static bool Update();
	static void Draw();
	static void End();

	static void SetCurrentScene(Scene* scene);

	static const PlatformInfo& GetPlatformInfo();

	static void PlatformUpdated();	

};