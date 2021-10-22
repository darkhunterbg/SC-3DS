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

#include <cstring>

static FILE* f;
static smk video;

static unsigned long w, h, frame, frames;
static double frameTime;
static uint8_t yScaling;

static  RenderSurface surface;

static double countdown = -1;

static 	std::vector<uint8_t> pixelData;

Coroutine crt;



static void ReadAndDecode()
{
	smk_next(video);

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

void VideoPlaybackScene::Start()
{
	f = Platform::OpenAsset("Blizzard.smk", AssetType::Unknown);
	video = smk_open_filepointer(f, SMK_MODE_DISK);



	smk_info_video(video, &w, &h, &yScaling);
	smk_info_all(video, &frame, &frames, &frameTime);

	frameTime /= 1000;

	smk_enable_video(video, true);

	smk_first(video);



	surface = GraphicsRenderer::NewRenderSurface({ (int)512,(int)512 });
	pixelData.resize(512 * 512 * 4);
	memset(pixelData.data(), 0, pixelData.size());

	crt = AssetLoader::RunIOAsync(ReadAndDecode);

	crt->RunAll();

	crt = nullptr;

	Platform::UpdateSurface(surface.surfaceId, { {0,0}, {512,512} }, { pixelData.data(), pixelData.size() });

	countdown += frameTime;

	crt = AssetLoader::RunIOAsync(ReadAndDecode);
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
			Platform::UpdateSurface(surface.surfaceId, { {0,0}, {512,512} }, { pixelData.data(), pixelData.size() });
		}

		crt = nullptr;
	}


	if (crt == nullptr)
	{
		crt = AssetLoader::RunIOAsync(ReadAndDecode);
	}

	GUIImage::DrawSubTexture(*surface.sprite.texture, { {0,0},{(int)w,(int)h} });
}

