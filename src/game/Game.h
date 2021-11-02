#pragma once

#include "Assets.h"
#include "Scenes/Scene.h"
#include "GameInputSchema.h"
#include "Engine/AudioChannel.h"

struct PlatformInfo;

struct GameStartSettings {
	bool skipIntro = false;
	bool mute = false;
	bool loadTestScene = false;
};

class Game {

private:
	Game() = delete;
	~Game() = delete;

	static GameInputSchema* _input;
	static bool _exit;
public:
	static const Font* SystemFont16;
	static const Font* SystemFont12;
	static const Font* SystemFont10;
	static const Font* SystemFont8;
	static AudioClip* ButtonAudio;

	static float DeltaTime;

	static bool ShowPerformanceStats;

	static void Start(GameStartSettings settings = {});
	static bool Frame();
	static void End();
	static void Exit();

	static void SetCurrentScene(Scene* scene);

	static Scene* GetCurrentScene();

	static const PlatformInfo& GetPlatformInfo();

	static void PlatformUpdated();	

	static GameInputSchema& GetInput();


	static const AudioChannelState* GetMusicChannel();
	static const AudioChannelState* GetUIChannel();
};