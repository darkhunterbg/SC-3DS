#include "VideoPlaybackScene.h"
#include "../GUI/GUI.h"
#include "../Platform.h"
#include "../Engine/GraphicsRenderer.h"

#include "../Scenes/BootScene.h"
#include "../Game.h"
#include "../Loader/smacker.h"
#include "../Profiler.h"

#include "../Debug.h"

#include "../Engine/AssetLoader.h"
#include "../Engine/AudioManager.h"
#include "../Engine/InputManager.h"

#include "../TimeSlice.h"
#include "../Engine/IAudioSource.h"

#include <cstring>

static FILE* f;
static smk video;

static  RenderSurface surface;

static double countdown = -1;

static uint8_t* pixelData;

Coroutine _readCrt;

static volatile bool read = false;
static volatile bool decoded = false;

static void Read()
{
	auto p = SectionProfiler("Read");
	smk_next(video);
	read = true;
}


static VideoClip* _clip;
static IAudioSource* _audioSrc;

static void Decode()
{
	auto p = SectionProfiler("Decode");
	
	_clip->DecodeCurrentFrame(pixelData, _clip->GetTextureSize().x);

	decoded = true;
}

void VideoPlaybackScene::Start()
{
	_clip = AssetLoader::LoadVideoClip("Blizzard");
	_audioSrc = _clip->PrepareAudio();

	Vector2Int size = _clip->GetTextureSize();
	surface = GraphicsRenderer::NewRenderSurface(size);

	pixelData = (uint8_t*)Platform::PlatformAlloc(size.x * size.y * 4);

	memset(pixelData, 0, size.x * size.y * 4);

	read = false;
	decoded = false;

	_readCrt = _clip->LoadFirstFrameAsync(&read);

}


void VideoPlaybackScene::Stop()
{
	Platform::PlatformFree(pixelData);
}

void VideoPlaybackScene::Draw()
{
	GUI::UseScreen(ScreenId::Top);
	GUIImage::DrawColor(Colors::Black);

	countdown -= 16.6f;// Game::DeltaTime * 1000;
	if (countdown <= 0)
	{
		if (_clip->IsAtEnd())
		{
			Game::SetCurrentScene(new BootScene());
			return;
		}

		countdown += _clip->GetFrameTime();

		if (!read)
			_readCrt->RunAll();

		if (!decoded)
			Decode();
		{
			auto p = SectionProfiler("Texture Update");
			Vector2Int size = _clip->GetTextureSize();
			Platform::UpdateSurface(surface.surfaceId, { {0,0}, size }, { pixelData,(unsigned)( size.x * size.y * 4 )});

		}

		_readCrt = nullptr;

		if (_audioSrc && _clip->GetCurrentFrame() == 1)
			AudioManager::Play(*_audioSrc, 0);
	}
	else
	{

		if (read && !decoded)
		{
			Decode();
		}

	}


	if (_readCrt == nullptr)
	{
		read = false;
		decoded = false;
		_readCrt = _clip->LoadNextFrameAsync(&read);
	}

	if (Game::GetPlatformInfo().Type == PlatformType::Nintendo3DS)
	{
		Vector2Int size = _clip->GetFrameSize();
		size.y /= 2;
		GUI::BeginRelativeLayout({ {0,0}, size });
	}
	else
	{
		Vector2Int size = _clip->GetFrameSize();
		size.x *= 2;
		GUI::BeginRelativeLayout({ {0,0},size });
	}

	Vector2Int size = _clip->GetFrameSize();
	GUIImage::DrawSubTexture(*surface.sprite.texture, { {0,0},size });
	GUI::EndLayout();

	if (InputManager::Gamepad.IsButtonDown(GamepadButton::A))
	{
		Game::SetCurrentScene(new BootScene());

	}
}

