#include "Game.h"

#include "Scenes/Scene.h"
#include "Scenes/GameScene.h"
#include "Scenes/PerformanceTestScene.h"
#include "StringLib.h"
#include "Profiler.h"
#include "Data/UnitDatabase.h"
#include "Data/GraphicsDatabase.h"
#include "Data/RaceDatabase.h"
#include "Data/WeaponDatabase.h"
#include "Data/AbilityDatabase.h"

#include "Engine/GraphicsRenderer.h"
#include "Engine/JobSystem.h"
#include "Engine/AudioManager.h"
#include "Engine/AssetLoader.h"
#include "Engine/InputManager.h"

#include "Platform.h"



static Scene* currentScene;

static const SpriteAtlas* title;
static bool startup;
static std::string error;
static double frameStartTime = 0;

const Font* Game::SystemFont12;
const Font* Game::SystemFont10;
const Font* Game::SystemFont8;


AudioClip Game::ButtonAudio;

float Game::DeltaTime = 0;


static void ShowTitleScreen() {
	GraphicsRenderer::DrawOnScreen(ScreenId::Top);
	GraphicsRenderer::Draw(title->GetSprite(0), { 0,0 });
}
static void InitialScene() {
	Game::SetCurrentScene(new GameScene());
	//Game::SetCurrentScene(new PerformanceTestScene());
}

void Game::FrameStart() {
	auto now = Platform::ElaspedTime();
	DeltaTime = now - frameStartTime;
	frameStartTime = now;

	Profiler::FrameStart();

}
void Game::FrameEnd() {
	Profiler::FrameEnd();
}

void Game::Start() {

	JobSystem::Init();
	SystemFont12 = AssetLoader::LoadFont("font.bcfnt",12);
	SystemFont10 = AssetLoader::LoadFont("font.bcfnt", 10);
	SystemFont8 = AssetLoader::LoadFont("font.bcfnt", 8);
	ButtonAudio = AssetLoader::LoadAudioClip("sound/misc/button.wav");
	title = AssetLoader::LoadAtlas("glue_title.t3x");
	startup = true;
	frameStartTime = Platform::ElaspedTime();
	AudioManager::Init();
	GraphicsRenderer::Init();


	GraphicsDatabase::Init();
	AbilityDatabase::Init();
	WeaponDatabase::Init();
	UnitDatabase::Init();
	RaceDatabase::Init();
}
bool Game::Update() {

	//SectionProfiler p("Update");

	
	InputManager::Update();

	AudioManager::UpdateAudio();

	if (startup) {

	}
	else {
		if (currentScene)
			currentScene->Update();
		else
			InitialScene();
	}

	//p.Submit();

	return true;
}

void Game::Draw() {
	//SectionProfiler p("Draw");
	GraphicsRenderer::NewFrame();

	if (startup) {
		ShowTitleScreen();
		startup = false;
	}
	else {
		if (currentScene)
			currentScene->Draw();
	}
	//p.Submit();

	Profiler::ShowPerformance();

	GraphicsRenderer::EndFrame();
}

void Game::End() {

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
