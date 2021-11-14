#include "MainMenuScene.h"
#include "../SceneView/IMainMenuSceneView.h"
#include "../Engine/AssetLoader.h"
#include "../Engine/AudioManager.h"
#include "../Game.h"

#include "../SceneView/MainMenuView.h"
#include "../SceneView/MultiplayerConnectionView.h"
#include "../Widgets/Cursor.h"
#include "../GUI/GUI.h"

#include "../Scenes/GameScene.h"

MainMenuScene::MainMenuScene()
{
	_id = NAMEOF(MainMenuScene);
}

MainMenuScene::~MainMenuScene()
{
}

void MainMenuScene::Start()
{
	auto clip = AssetLoader::LoadAudioClip("music\\title");
	if (AudioManager::GetAudioChannels()[0].GetStream() != clip)
		AudioManager::Play(*clip, Game::GetMusicChannel());

	cursor = new Cursor();

	ToMainMenu();
	//ToMultiplayerConnection();
}

void MainMenuScene::Stop()
{
	if (_view)
		delete _view;

	if (cursor)
		delete cursor;


	// TODO: unload font
}

void MainMenuScene::Frame(TimeSlice& frameBudget)
{
	if (_nextScene)
	{
		bool change = true;

		if (_view)
		{
			change = _view->DoneHiding();
		}

		if (change)
		{
			Game::SetCurrentScene(_nextScene);
		}
	}

	if (_nextView)
	{
		bool change = true;

		if (_view)
		{
			change = _view->DoneHiding();
			if(change)
				delete _view;
		
			GUI::CleanResources();
		}

		if (change)
		{
			_view = _nextView;
			_nextView = nullptr;
			_state = _nextState;

			_view->OnShow();
		}
	}

	if (_view != nullptr)
		_view->Draw();


	if (Game::GetInput().IsUsingMouse())
	{
		cursor->Update();
		cursor->Draw();
	}
}


void MainMenuScene::SwitchView(MainMenuState state, IMainMenuSceneView* view)
{
	_nextView = view;
	_nextState = state;
}

void MainMenuScene::ToMainMenu()
{
	SwitchView(MainMenuState::MainMenu, new MainMenuView(this));
}

void MainMenuScene::ToMultiplayerConnection()
{
	SwitchView(MainMenuState::MultiplayerSelection, new MultiplayerConnectionView(this));
}

void MainMenuScene::StartGame()
{
	if (_nextScene)
		delete _nextScene;
	_nextScene = nullptr;
	_nextScene = new GameScene();
}
