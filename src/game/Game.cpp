#include "Game.h"

#include "Scenes/Scene.h"
#include "Scenes/GameScene.h"
#include "Scenes/BootScene.h"
#include "Scenes/VideoPlaybackScene.h"
#include "StringLib.h"
#include "Profiler.h"

#include "Engine/GraphicsRenderer.h"
#include "Engine/JobSystem.h"
#include "Engine/AudioManager.h"
#include "Engine/AssetLoader.h"
#include "Engine/InputManager.h"

#include "GUI/GUI.h"

#include "Platform.h"

#include <queue>

static Scene* currentScene;

static std::string error;
static double frameStartTime = 0;

const Font* Game::SystemFont16;
const Font* Game::SystemFont12;
const Font* Game::SystemFont10;
const Font* Game::SystemFont8;
GameInputSchema* Game::_input;

AudioClip* Game::ButtonAudio;

float Game::DeltaTime = 0;
bool Game::ShowPerformanceStats = true;

static PlatformInfo _platformInfo;


static std::queue<Scene*> _nextScene;

static bool SwithScenes()
{
	bool switched = false;

	while (_nextScene.size() > 0)
	{
		if (currentScene != nullptr)
		{
			currentScene->Stop();
			delete currentScene;

			GUI::CleanResources();
		}

		currentScene = _nextScene.back();
		_nextScene.pop();
		currentScene->Start();

		switched = true;
	}

	return switched;
}


static void InitialScene(GameStartSettings settings)
{
	Scene* afterBootScene = settings.loadTestScene ? new GameScene() : nullptr;

	if (settings.skipIntro)
	{
		Game::SetCurrentScene(new BootScene(afterBootScene));
	}
	else
	{
		Game::SetCurrentScene(new VideoPlaybackScene("Smk\\Blizzard", [afterBootScene]() {
			Game::SetCurrentScene(new BootScene(afterBootScene));
			}));

	}

	SwithScenes();
}

void Game::FrameStart()
{
	auto now = Platform::ElaspedTime();
	DeltaTime = now - frameStartTime;
	frameStartTime = now;

	Profiler::FrameStart();

}
void Game::FrameEnd()
{
	Profiler::FrameEnd();
}

void Game::Start(GameStartSettings settings)
{
	_platformInfo = Platform::GetPlatformInfo();

	JobSystem::Init();

	AssetLoader::Init();

	SystemFont16 = AssetLoader::LoadFont("font", 16);
	SystemFont12 = AssetLoader::LoadFont("font", 12);
	//SystemFont10 = AssetLoader::LoadFont("font.bcfnt", 10);
	//SystemFont8 = AssetLoader::LoadFont("font.bcfnt", 8);
	//ButtonAudio = AssetLoader::LoadAudioClip("sound\\misc\\button");
	frameStartTime = Platform::ElaspedTime();
	AudioManager::Init();
	AudioManager::SetMute(settings.mute);
	GraphicsRenderer::Init();
	GUI::SetState(*(new GUIState()));

	_input = new GameInputSchema();
	_input->InitDefault();

	InitialScene(settings);
}
bool Game::Update()
{
	//SectionProfiler p("Update");

	SwithScenes();

	InputManager::Update();

	AudioManager::UpdateAudio();

	if (currentScene)
		currentScene->Update();

	if (SwithScenes())
	{
		// Run another update for the new scene
		if (currentScene)
			currentScene->Update();
	}

	//p.Submit();

	return true;
}

void Game::Draw()
{
	//SectionProfiler p("Draw");
	GraphicsRenderer::NewFrame();

	if (currentScene)
		currentScene->Draw();

	//p.Submit();

	if (ShowPerformanceStats)
		Profiler::ShowPerformance();

	GraphicsRenderer::EndFrame();
}

void Game::End()
{
	if (currentScene != nullptr)
		currentScene->Stop();

	currentScene = nullptr;


	GUI::CleanResources();

}

void Game::SetCurrentScene(Scene* scene)
{
	_nextScene.emplace(scene);

}

Scene* Game::GetCurrentScene()
{
	return currentScene;
}

const PlatformInfo& Game::GetPlatformInfo()
{
	return _platformInfo;
}

void Game::PlatformUpdated()
{
	_platformInfo = Platform::GetPlatformInfo();

	if (currentScene)
		currentScene->OnPlatformChanged();
}

GameInputSchema& Game::GetInput()
{
	return *_input;
}

