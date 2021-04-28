#include "Game.h"

#include "Platform.h"
#include "Scenes/Scene.h"
#include "Scenes/GameScene.h"
#include "StringLib.h"
#include "Audio.h"

static Scene* currentScene;

static double updateTime;
static double drawTime;
static double frameTime;


static const SpriteAtlas* title;
static bool startup;
static std::string error;
static double frameStartTime = 0;

Font Game::SystemFont;
GamepadState Game::Gamepad;
PointerState Game::Pointer;
AudioSystem* Game::Audio = nullptr;
float Game::DeltaTime = 0;


static void ShowPerformance() {
	Platform::DrawOnScreen(ScreenId::Top);

	char data[128];
	stbsp_snprintf(data, sizeof(data), "Update %.2f ms", updateTime);
	Platform::DrawText(Game::SystemFont, { 1,0 }, data, Colors::White, 0.4f);

	stbsp_snprintf(data, sizeof(data), "Draw %.2f ms", drawTime);
	Platform::DrawText(Game::SystemFont, { 1,15 }, data, Colors::White, 0.4f);

	stbsp_snprintf(data, sizeof(data), "Frame %.2f ms", frameTime);
	Platform::DrawText(Game::SystemFont, { 1,30 }, data, Colors::White, 0.4f);

	stbsp_snprintf(data, sizeof(data), "Time %i s", (int)Platform::ElaspedTime());
	Platform::DrawText(Game::SystemFont, { 1,45 }, data, Colors::White, 0.4f);
}
static void ShowTitleScreen() {
	Platform::DrawOnScreen(ScreenId::Top);
	Platform::Draw(title->GetSprite(0), { {0,0},{400,240} }, Colors::White);
}
static void InitialScene() {
	Game::SetCurrentScene(new GameScene());
}

void Game::FrameStart() {
	auto now = Platform::ElaspedTime();
	DeltaTime = now - frameStartTime;
	frameStartTime = now;
}
void Game::FrameEnd() {
	auto now = Platform::ElaspedTime();
	frameTime = (now - frameStartTime) * 1000;
}

void Game::Start() {
	SystemFont = Platform::LoadFont("font.bcfnt");
	title = Platform::LoadAtlas("glue_title.t3x");
	startup = true;
	frameStartTime = Platform::ElaspedTime();
	Audio = new AudioSystem();
}
bool Game::Update() {

	double start = Platform::ElaspedTime();

	Platform::UpdateGamepadState(Gamepad);
	Platform::UpdatePointerState(Pointer);
	Audio->UpdateAudio();

	if (startup) {
	
	}
	else {
		if (currentScene)
			currentScene->Update();
		else
			InitialScene();
	}

	updateTime = (Platform::ElaspedTime() - start) * 1000.0;

	return true;
}

void Game::Draw() {
	double start = Platform::ElaspedTime();

	if (startup) {
		ShowTitleScreen();
		startup = false;
	}
	else {
		if (currentScene)
			currentScene->Draw();
	}

	drawTime = (Platform::ElaspedTime() - start) * 1000.0;

	ShowPerformance();
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
