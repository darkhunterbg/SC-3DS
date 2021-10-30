#include "BootScene.h"
#include "../Engine/AssetLoader.h"
#include "../Engine/AudioManager.h"
#include "../Data/GameDatabase.h"
#include "../Game.h"
#include "../Scenes/GameScene.h"
#include "../TimeSlice.h"
#include "../SceneView/BootSceneView.h"
#include "MainMenuScene.h"


static constexpr const bool AutoStart = true;

BootScene::BootScene(Scene* next)
{
	_id = NAMEOF(BootScene);

	_nextScene = next;
	if (_nextScene == nullptr)
	{
		_nextScene = new MainMenuScene();
	}

#if(_DEBUG)
	_nextSceneDelay = 0;
#endif
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
	AudioManager::Play(*clip, Game::GetMusicChannel());

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
		if (_loadCrt->IsCompleted())
		{
			if (_nextSceneDelay > 0)
				--_nextSceneDelay;
			else
				_ready = true;
		}
		else
		{
			TimeSlice budget = TimeSlice(0.010);

			while (!budget.IsTimeElapsed())
			{
				if (_loadCrt->Next()) break;
				
			}
		}
	}


	if (_ready && AutoStart)
	{
		StartGame();
	}
}

void BootScene::Draw()
{
	_view->Draw();
}
