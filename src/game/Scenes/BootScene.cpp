#include "BootScene.h"
#include "../Engine/AssetLoader.h"
#include "../Engine/AudioManager.h"
#include "../Data/GameDatabase.h"
#include "../Game.h"
#include "../Scenes/GameScene.h"
#include "../TimeSlice.h"
#include "../SceneView/BootSceneView.h"
#include "MainMenuScene.h"

static bool _autoStart = true;

BootScene::BootScene(Scene* next)
{
	_id = NAMEOF(BootScene);

	_nextScene = next;
	if (_nextScene == nullptr)
	{
		_nextScene = new MainMenuScene();
	}
}

BootScene::~BootScene()
{

}

void BootScene::StartGame()
{
	Game::SetCurrentScene(_nextScene);
}

void BootScene::Start()
{
	_view = new BootSceneView(this);

	auto clip = AssetLoader::LoadAudioClip("music\\title");
	AudioManager::Play(*clip, 0);

	_loadCrt = AssetLoader::LoadDatabaseAsync();

}

void BootScene::Stop()
{
	delete _view;
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
