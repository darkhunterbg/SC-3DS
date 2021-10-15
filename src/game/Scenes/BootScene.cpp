#include "BootScene.h"
#include "../Engine/AssetLoader.h"
#include "../Engine/AudioManager.h"
#include "../Engine/GraphicsRenderer.h"
#include "../Data/GameDatabase.h"
#include "../Game.h"
#include "../Scenes/GameScene.h"
#include "../Platform.h"

const Texture* _title;

BootScene::BootScene()
{
	_title = AssetLoader::LoadTexture("atlases\\title_0");

}
void BootScene::Start()
{
	auto clip = AssetLoader::LoadAudioClip("music\\title");
	AudioManager::PlayClip(clip, 0);
}

void BootScene::Stop()
{
	if (_loadingCrt != nullptr)
		delete _loadingCrt;

	delete this;
}

void BootScene::Update()
{
	if (_loadingCrt == nullptr)
	{
		_loadingCrt = AssetLoader::LoadDatabaseAsync();
	}
	else
	{
		if (!_loadingCrt->MoveNext())
		{
			//delete _loadingCrt;
			Game::SetCurrentScene(new GameScene());
		}
	}

	//if (_frameCounter == 1)
	//{
	//	AssetLoader::LoadDatabase();
	//}
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

		GraphicsRenderer::DrawText(*Game::SystemFont16, pos, "Loading", Colors::SCWhite);
	}



	_frameCounter++;
}



BootScene::~BootScene()
{

}
