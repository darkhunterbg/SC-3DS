#include "MainMenuScene.h"
#include "../SceneView/MainMenuSceneView.h"
#include "../Engine/AssetLoader.h"
#include "../Engine/AudioManager.h"
#include "../Game.h"

MainMenuScene::MainMenuScene()
{
	_id = NAMEOF(MainMenuScene);
}

MainMenuScene::~MainMenuScene()
{
}

void MainMenuScene::Start()
{
	_view = new MainMenuSceneView(this);
	auto clip = AssetLoader::LoadAudioClip("music\\title");
	if (AudioManager::GetAudioChannels()[0].GetStream() != clip)
		AudioManager::Play(*clip, Game::GetMusicChannel());
}

void MainMenuScene::Stop()
{
	delete _view;
}

void MainMenuScene::Draw()
{
	_view->Draw();
}
