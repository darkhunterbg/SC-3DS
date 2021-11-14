#include "Game.h"

#include "Scenes/Scene.h"
#include "Scenes/GameScene.h"
#include "Scenes/BootScene.h"
#include "Scenes/VideoPlaybackScene.h"
#include "Scenes/MainMenuScene.h"
#include "StringLib.h"
#include "Profiler.h"

#include "Engine/GraphicsRenderer.h"
#include "Engine/JobSystem.h"
#include "Engine/AudioManager.h"
#include "Engine/AssetLoader.h"
#include "Engine/InputManager.h"
#include "TimeSlice.h"

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
const Font* Game::MenuFont16;
GameInputSchema* Game::_input;
bool Game::_exit = false;


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
		Game::ShowIntro();
	}


	SwithScenes();
}



void Game::Start(GameStartSettings settings)
{
	_exit = false;
	_platformInfo = Platform::GetPlatformInfo();

	JobSystem::Init();

	AssetLoader::Init();

	SystemFont16 = AssetLoader::LoadFont("font", 16);
	SystemFont12 = AssetLoader::LoadFont("font", 12);
	SystemFont10 = AssetLoader::LoadFont("font", 10);
	SystemFont8 = AssetLoader::LoadFont("font", 8);
	MenuFont16 = AssetLoader::LoadFont("mm-font", 16);
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
bool Game::Frame()
{
	Profiler::FrameStart();

	auto now = Platform::ElaspedTime();
	DeltaTime = now - frameStartTime;
	frameStartTime = now;

	if (_exit) return false;

	TimeSlice frameBudget(0.016);

	SwithScenes();

	InputManager::Update();

	AudioManager::UpdateAudio();

	if (currentScene)
		currentScene->Frame(frameBudget);

	if (_exit) return false;

	SwithScenes();

	if (_exit) return false;

	InputManager::FrameEnd();

	GUI::FrameEnd();

	Profiler::FrameEnd();

	if (ShowPerformanceStats)
		Profiler::ShowPerformance();

	return !_exit;
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

void Game::Exit()
{
	_exit = true;
}

const AudioChannelState* Game::GetMusicChannel()
{
	if (AudioManager::GetAudioChannels().Size() > 0)
		return	&AudioManager::GetAudioChannels()[0];

	return nullptr;
}

const AudioChannelState* Game::GetUIMonoChannel()
{
	int size = AudioManager::GetAudioChannels().Size();
	if (size > 1)
		return &AudioManager::GetAudioChannels()[size - 2];

	return nullptr;
}

const AudioChannelState* Game::GetUIStereoChannel()
{
	int size = AudioManager::GetAudioChannels().Size();
	if (size > 1)
		return &AudioManager::GetAudioChannels()[size - 1];

	return nullptr;
}

void Game::PlayUISound(const SoundSetDef* def)
{
	if (def == nullptr) return;

	if (def->ClipCount == 0) return;

	auto& clip = def->GetAudioClip(0);
	if (clip.IsMono())
		AudioManager::Play(clip, GetUIMonoChannel());
	else
		AudioManager::Play(clip, GetUIStereoChannel());
}

void Game::ShowIntro()
{
	Game::SetCurrentScene(new VideoPlaybackScene("Smk\\Blizzard", []() {

		if (GameDatabase::instance == nullptr)
			Game::SetCurrentScene(new BootScene());
		else
			Game::SetCurrentScene(new MainMenuScene());
		}));
}
