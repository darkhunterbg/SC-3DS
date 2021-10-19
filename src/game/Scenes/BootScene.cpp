#include "BootScene.h"
#include "../Engine/AssetLoader.h"
#include "../Engine/AudioManager.h"
#include "../Engine/GraphicsRenderer.h"
#include "../Engine/InputManager.h"
#include "../Engine/JobSystem.h"
#include "../Data/GameDatabase.h"
#include "../Game.h"
#include "../Scenes/GameScene.h"
#include "../Platform.h"

static const Texture* _title;
static bool _autoStart = true;

BootScene::BootScene()
{
	_title = AssetLoader::LoadTexture("atlases\\title_0");

}
void BootScene::Start()
{
	auto clip = AssetLoader::LoadAudioClip("music\\title");
	AudioManager::PlayClip(clip, 0);

	//JobSystem::RunJobAsync(1, 1, [](int, int) {
		AssetLoader::LoadDatabase();
		//});

}

void BootScene::Stop()
{

}

void BootScene::Update()
{
	if (!_ready && !JobSystem::IsWorking())
	{
		_ready = true;
	}
}

void BootScene::Draw()
{

	if (Game::GetPlatformInfo().Type == PlatformType::Nintendo3DS)
	{
		GraphicsRenderer::Draw(*_title, { 0,0 }, { 400,300 });

		GraphicsRenderer::DrawOnScreen(ScreenId::Bottom);

		GraphicsRenderer::DrawRectangle({ {0,0},{320,240} }, Colors::Black);

		GraphicsRenderer::Draw(*_title, { -430,-358 }, { 640,480 });
		GraphicsRenderer::DrawRectangle({ {0,0},{120,200} }, Colors::Black);

		GraphicsRenderer::Draw(*_title, { -40,-240 }, { 400,300 });
		GraphicsRenderer::DrawRectangle({ {300,25},{20,100} }, Colors::Black);

		if (_ready)
		{
			if ((_frameCounter / 30) % 3)

				GraphicsRenderer::DrawText(*Game::SystemFont12, { 110,140 }, "Press A to continue", Colors::SCWhite);
		}
		else
			GraphicsRenderer::DrawText(*Game::SystemFont12, { 145,140 }, "Loading", Colors::SCWhite);
	}
	else
	{
		GraphicsRenderer::DrawOnScreen(ScreenId::Top);

		GraphicsRenderer::Draw(*_title, { 0,0 }, Vector2Int(_title->GetSize()));

		Vector2Int pos = Vector2Int(_title->GetSize());
		pos.x /= 2;
		pos.y -= 90;
		pos.x -= 22;

		if (_ready)
		{
			if ((_frameCounter / 30) % 3)

				GraphicsRenderer::DrawText(*Game::SystemFont16, pos - Vector2Int{ 40, 0 }, "Press A to continue", Colors::SCWhite);
		}
		else
			GraphicsRenderer::DrawText(*Game::SystemFont16, pos, "Loading", Colors::SCWhite);
	}


	if (_ready && (_autoStart || InputManager::Gamepad.IsButtonDown(GamepadButton::A)))
	{
		Game::SetCurrentScene(new GameScene());
	}

	_frameCounter++;
}



BootScene::~BootScene()
{

}
