#include "BootScene.h"
#include "../Engine/AssetLoader.h"
#include "../Engine/AudioManager.h"
#include "../Data/GameDatabase.h"
#include "../Game.h"
#include "../Scenes/GameScene.h"
#include "../TimeSlice.h"
#include "../SceneView/BootSceneView.h"

static bool _autoStart = false;

BootScene::BootScene()
{
	_view = new BootSceneView(this);
}

BootScene::~BootScene()
{
	delete _view;
}

void BootScene::StartGame()
{
	Game::SetCurrentScene(new GameScene());
}

void BootScene::Start()
{
	auto clip = AssetLoader::LoadAudioClip("music\\title");
	AudioManager::PlayClip(clip, 0);

	_loadCrt = AssetLoader::LoadDatabaseAsync();

}

void BootScene::Stop()
{
}

void BootScene::Update()
{
	if (!_ready)
	{
		TimeSlice budget = TimeSlice(0.010);

		while (!budget.IsTimeElapsed())
		{
			if (_loadCrt->Next())
			{
				_ready = true;

				break;
			}
		}
	}


	if (_ready && _autoStart)
	{
		StartGame();
	}
}

void BootScene::Draw()
{
	_view->Draw();
}
