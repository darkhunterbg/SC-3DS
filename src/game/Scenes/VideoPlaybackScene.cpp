#include "VideoPlaybackScene.h"
#include "../GUI/GUI.h"
#include "../Platform.h"
#include "../Engine/GraphicsRenderer.h"

#include "../Scenes/BootScene.h"
#include "../Game.h"
#include "../LibSmacker/smacker.h"
#include "../Profiler.h"

#include "../Debug.h"

#include "../Engine/AssetLoader.h"
#include "../Engine/AudioManager.h"
#include "../Engine/InputManager.h"

#include "../Engine/IAudioSource.h"

#include <cstring>

static FILE* f;
static smk video;

static unsigned long w, h, frame, frames;
static double frameTime;
static uint8_t yScaling;

static  RenderSurface surface;

static double countdown = -1;

static uint8_t* pixelData;

Coroutine crt;

static unsigned char   a_trackmask, a_channels[7], a_depth[7];
static unsigned long   a_rate[7];

static BufferedAudioSource audioSrc;

#ifdef _3DS
#include <3ds.h>
#endif

static void ReadAndDecode()
{
	
	{
		auto p = SectionProfiler("Read");
		smk_next(video);
	}

	auto p = SectionProfiler("Decode");
		const uint8_t* pal_data = smk_get_palette(video);
		const uint8_t* image = smk_get_video(video);
		int a = 0;
		for (int y = 0; y < h; ++y)
		{
			for (int x = 0; x < w; ++x)
			{
				int i = x + y * 512;

				int index = image[++a];
				index *= 3;

				pixelData[i * 4 + 2] = (pal_data[index]);
				pixelData[i * 4 + 3] = (pal_data[index + 1]);
				pixelData[i * 4 + 1] = (pal_data[index + 2]);
				pixelData[i * 4] = 255;
			}
		}
	

}

static std::vector< uint8_t> audioStream;

void VideoPlaybackScene::Start()
{
	f = Platform::OpenAsset("Blizzard.smk", AssetType::Unknown);
	video = smk_open_filepointer(f, SMK_MODE_DISK);


	smk_info_audio(video, &a_trackmask, a_channels, a_depth, a_rate);
	smk_info_video(video, &w, &h, &yScaling);
	smk_info_all(video, &frame, &frames, &frameTime);

	frameTime /= 1000;

	smk_enable_audio(video, 0, true);

	smk_first(video);

	auto size = smk_get_audio_size(video, 0);


	for (int i = 0; i < frames; ++i)
	{
		const uint8_t* audio = smk_get_audio(video, 0);
		size = smk_get_audio_size(video, 0);
		int offset = audioStream.size();
		audioStream.resize(offset + size);


		memcpy(audioStream.data() + offset, audio, size);
		smk_next(video);
	}
	audioSrc.AddAndOwnBuffer(audioStream.data(), audioStream.size());

	smk_enable_video(video, true);
	smk_enable_audio(video, 0, false);


	smk_first(video);

	surface = GraphicsRenderer::NewRenderSurface({ (int)512,(int)512 });

#ifdef _3DS

	pixelData = (uint8_t*)linearAlloc(512 * 512 * 4);
#else
	pixelData = new uint8_t[512 * 512 * 4];
#endif
	memset(pixelData, 0, 512 * 512 * 4);

	crt = AssetLoader::RunIOAsync(ReadAndDecode);

	crt->RunAll();

	crt = nullptr;

	Platform::UpdateSurface(surface.surfaceId, { {0,0}, {512,512} }, { pixelData,512 * 512 * 4 });

	countdown += frameTime;

	crt = AssetLoader::RunIOAsync(ReadAndDecode);

	AudioManager::Play(audioSrc, 0);
}
void VideoPlaybackScene::Stop()
{

}

void VideoPlaybackScene::Draw()
{
	GUI::UseScreen(ScreenId::Top);
	GUIImage::DrawColor(Colors::Black);

	countdown -= 16.6;
	if (countdown <= 0)
	{
		smk_info_all(video, &frame, nullptr, nullptr);

		if (frame >= frames - 1)
		{
			Game::SetCurrentScene(new BootScene());
			return;
		}

		countdown += frameTime;

		crt->RunAll();

		{
			auto p = SectionProfiler("Texture Update");
			Platform::UpdateSurface(surface.surfaceId, { {0,0}, {512,512} }, { pixelData, 512 * 512 * 4 });
		}

		//const uint8_t* audio = smk_get_audio(video, 0);
		//auto size = smk_get_audio_size(video, 0);



		//AudioManager::UpdateAudio();

		crt = nullptr;
	}


	if (crt == nullptr)
	{
		crt = AssetLoader::RunIOAsync(ReadAndDecode);
	}

	GUIImage::DrawSubTexture(*surface.sprite.texture, { {0,0},{(int)w,(int)h} });

	if (InputManager::Gamepad.IsButtonDown(GamepadButton::A))
	{
		Game::SetCurrentScene(new BootScene());

	}
}

