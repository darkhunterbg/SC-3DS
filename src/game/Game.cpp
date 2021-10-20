#include "Game.h"

#include "Scenes/Scene.h"
#include "Scenes/GameScene.h"
#include "Scenes/BootScene.h"
#include "StringLib.h"
#include "Profiler.h"

#include "Engine/GraphicsRenderer.h"
#include "Engine/JobSystem.h"
#include "Engine/AudioManager.h"
#include "Engine/AssetLoader.h"
#include "Engine/InputManager.h"


#include "Platform.h"



static Scene* currentScene;

static std::string error;
static double frameStartTime = 0;

const Font* Game::SystemFont16;
const Font* Game::SystemFont12;
const Font* Game::SystemFont10;
const Font* Game::SystemFont8;

AudioClip* Game::ButtonAudio;

float Game::DeltaTime = 0;

static PlatformInfo _platformInfo;


static void InitialScene()
{
	Game::SetCurrentScene(new BootScene());
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

void Game::Start()
{
	_platformInfo = Platform::GetPlatformInfo();

	JobSystem::Init();

	AssetLoader::Init();

	SystemFont16 = AssetLoader::LoadFont("font.bcfnt", 16);
	SystemFont12 = AssetLoader::LoadFont("font.bcfnt", 12);
	//SystemFont10 = AssetLoader::LoadFont("font.bcfnt", 10);
	//SystemFont8 = AssetLoader::LoadFont("font.bcfnt", 8);
	//ButtonAudio = AssetLoader::LoadAudioClip("sound\\misc\\button");
	frameStartTime = Platform::ElaspedTime();
	AudioManager::Init();
	GraphicsRenderer::Init();

	InitialScene();
}
bool Game::Update()
{
	//SectionProfiler p("Update");

	InputManager::Update();

	AudioManager::UpdateAudio();

	if (currentScene)
		currentScene->Update();

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

	Profiler::ShowPerformance();

	GraphicsRenderer::EndFrame();
}

void Game::End()
{

	if (currentScene != nullptr)
		currentScene->Stop();

	currentScene = nullptr;
}

void Game::SetCurrentScene(Scene* scene)
{
	if (currentScene != nullptr)
		currentScene->Stop();

	currentScene = scene;
	currentScene->Start();
}

const PlatformInfo& Game::GetPlatformInfo()
{
	return _platformInfo;
}

void Game::PlatformUpdated()
{
	_platformInfo = Platform::GetPlatformInfo();
}
