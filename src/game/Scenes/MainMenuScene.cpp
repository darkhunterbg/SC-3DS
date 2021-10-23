#include "MainMenuScene.h"
#include "../SceneView/MainMenuSceneView.h"
#include "../Engine/AssetLoader.h"
#include "../Engine/AudioManager.h"

void MainMenuScene::Start()
{
	_view = new MainMenuSceneView(this);
	auto clip = AssetLoader::LoadAudioClip("music\\title");
	if (AudioManager::GetAudioChannels()[0].stream != clip)
		AudioManager::Play(*clip, 0);
}

void MainMenuScene::Stop()
{
	delete _view;
}

void MainMenuScene::Draw()
{
	_view->Draw();
}
