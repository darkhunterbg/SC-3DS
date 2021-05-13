#include "Game.h"

#include "Platform.h"
#include "Scenes/Scene.h"
#include "Scenes/GameScene.h"
#include "Scenes/PerformanceTestScene.h"
#include "StringLib.h"
#include "Audio.h"
#include "Profiler.h"
#include "Data/UnitDatabase.h"
#include "Job.h"

static Scene* currentScene;

static const SpriteAtlas* title;
static bool startup;
static std::string error;
static double frameStartTime = 0;

Font Game::SystemFont;
Gamepad Game::Gamepad;
Pointer Game::Pointer;
AudioSystem Game::Audio;
AssetLoader Game::AssetLoader;
float Game::DeltaTime = 0;



static void ShowTitleScreen() {
	Platform::DrawOnScreen(ScreenId::Top);
	Platform::Draw(title->GetSprite(0), { {0,0},{400,240} });
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
	SystemFont = AssetLoader.LoadFont("font.bcfnt");
	title = AssetLoader.LoadAtlas("glue_title.t3x");
	startup = true;
	frameStartTime = Platform::ElaspedTime();
	Audio.Init();
	UnitDatabase::Init();
	//frameLoad.push_back(0);
}
bool Game::Update() {

	//SectionProfiler p("Update");

	Gamepad.Update();
	Pointer.Update();
	Audio.UpdateAudio();

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
