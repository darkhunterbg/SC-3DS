#include "Game.h"

#include "Platform.h"
#include "Scenes/Scene.h"
#include "Scenes/GameScene.h"
#include "StringLib.h"
#include "Audio.h"
#include <vector>
#include <algorithm>

static Scene* currentScene;

struct FrameProfileData {

	double updateTime;
	double drawTime;
	double frameTime;
};




static const SpriteAtlas* title;
static bool startup;
static std::string error;
static double frameStartTime = 0;

Font Game::SystemFont;
GamepadState Game::Gamepad;
PointerState Game::Pointer;
AudioSystem* Game::Audio = nullptr;
float Game::DeltaTime = 0;

static FrameProfileData avgFrame = { 0,0,0 }, currentFrame = { 0,0,0 };
static std::vector<double> frameLoad;

static void ShowPerformance() {
	Platform::DrawOnScreen(ScreenId::Top);

	FrameProfileData data = avgFrame;

	char text[128];
	stbsp_snprintf(text, sizeof(text), "Update %.2f ms", data.updateTime);
	Platform::DrawText(Game::SystemFont, { 1,0 }, text, Colors::White, 0.4f);

	stbsp_snprintf(text, sizeof(text), "Draw %.2f ms", data.drawTime);
	Platform::DrawText(Game::SystemFont, { 1,15 }, text, Colors::White, 0.4f);

	stbsp_snprintf(text, sizeof(text), "Frame %0.2f ms", data.frameTime);
	Platform::DrawText(Game::SystemFont, { 1,30 }, text, Colors::White, 0.4f);

	stbsp_snprintf(text, sizeof(text), "Time %i s", (int)Platform::ElaspedTime());
	Platform::DrawText(Game::SystemFont, { 1,45 }, text, Colors::White, 0.4f);


	Vector2Int offset = { 1,110 };

	float scale = 1;
	float max = *std::max_element(frameLoad.begin(), frameLoad.end());
	while (max > scale) {
		scale += 1;
	}

	for (int i = 0; i < frameLoad.size(); ++i) {
		int y = std::fmax(1, ((frameLoad[i]) * 50 / (int)scale));
		Color c = Colors::LightGreen;
		if (frameLoad[i] > 1)
			c = Colors::Orange;
		if (frameLoad[i] > 2)
			c = Colors::Red;
		Platform::DrawLine(offset, offset + Vector2Int(0, -y), c);
		offset.x++;
	}
	offset = { 1,60 };

	Platform::DrawLine(offset, offset + Vector2Int(60,0 ), Colors::White);
	offset.y += 50;
	Platform::DrawLine(offset, offset + Vector2Int(60,0), Colors::White);
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
	currentFrame.frameTime = (now - frameStartTime) * 1000;
	frameLoad.push_back(currentFrame.frameTime / 16.66);
	if (frameLoad.size() >= 60)
		frameLoad.erase(frameLoad.begin());

	if (avgFrame.frameTime == 0.0)
		avgFrame = currentFrame;
	else {
		avgFrame.drawTime = avgFrame.drawTime * 0.95 + currentFrame.drawTime * 0.05;
		avgFrame.updateTime = avgFrame.updateTime * 0.95 + currentFrame.updateTime * 0.05;
		avgFrame.frameTime = avgFrame.frameTime * 0.95 + currentFrame.frameTime * 0.05;
	}

	currentFrame = { 0.0,0.0,0.0 };
}

void Game::Start() {

	SystemFont = Platform::LoadFont("font.bcfnt");
	title = Platform::LoadAtlas("glue_title.t3x");
	startup = true;
	frameStartTime = Platform::ElaspedTime();
	Audio = new AudioSystem();
	frameLoad.push_back(0);
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

	currentFrame.updateTime = (Platform::ElaspedTime() - start) * 1000.0;

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

	currentFrame.drawTime = (Platform::ElaspedTime() - start) * 1000.0;

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
