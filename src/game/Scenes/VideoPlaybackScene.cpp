#include "VideoPlaybackScene.h"
#include "../GUI/GUI.h"
#include "../Platform.h"

#include "../Game.h"
#include "../Profiler.h"

#include "../Debug.h"

#include "../Engine/AssetLoader.h"
#include "../Engine/AudioManager.h"

#include <cstring>

VideoPlaybackScene::VideoPlaybackScene(const char* video, std::function<void()> doneCallback)
{
	_videoPath = video;
	_onDoneCallback = doneCallback;

	_id = NAMEOF(VideoPlaybackScene);
}

void VideoPlaybackScene::Start()
{
	_clip = AssetLoader::LoadVideoClip(_videoPath.data());
}

void VideoPlaybackScene::Stop()
{
	if (_clip->HasAudio())
		AudioManager::StopAll();

	AssetLoader::UnloadVideoClip(_clip);
}

void VideoPlaybackScene::Frame()
{
	GUI::UseScreen(ScreenId::Top);
	GUIImage::DrawColor(Colors::Black);

	Vector2 scale = { 2,1 };

	if (Game::GetPlatformInfo().Type == PlatformType::Nintendo3DS)
	{
		scale = { 1,0.5f };
	}

	bool done = GUIVideo::DrawVideoScaled("video", * _clip, scale);

	GUI::EndLayout();

	if (done || Game::GetInput().Common.Comfirm.IsActivated())
	{
		_onDoneCallback();
	}

	if (Game::GetPlatformInfo().Type == PlatformType::Nintendo3DS)
	{
		GUI::UseScreen(ScreenId::Bottom);
		GUIImage::DrawColor(Colors::Black);
	}

}

