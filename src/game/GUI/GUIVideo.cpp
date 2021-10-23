#include "GUIVideo.h"
#include "GUIImage.h"
#include "../Platform.h"
#include "../Profiler.h"
#include "../Engine/GraphicsRenderer.h"
#include "../Engine/AudioManager.h"
#include "../Coroutine.h"

#include <cstring>

class VideoFrameRenderData {
public:
	uint8_t* pixelData = nullptr;
	unsigned pixelDataSize = 0;
	bool decoded = false;
	Texture* texture = nullptr;
	long currentFrame = -1;
	double countdown = 0;
	Coroutine readCrt;
	IAudioSource* audioSrc = nullptr;
	bool loaded = false;

	VideoFrameRenderData(Vector2Int size)
	{
		pixelDataSize = size.x * size.y * 4;
		pixelData = (uint8_t*)Platform::PlatformAlloc(pixelDataSize);
		memset(pixelData, 0, pixelDataSize);
	}

	~VideoFrameRenderData()
	{
		if (pixelData != nullptr)
			Platform::PlatformFree(pixelData);

		if (texture != nullptr)
			delete texture;
	}
};

static void Decode(VideoClip& clip, VideoFrameRenderData* data)
{
	auto p = SectionProfiler("Decode");

	clip.DecodeCurrentFrame(data->pixelData, clip.GetTextureSize().x);

	data->decoded = true;
}

bool GUIVideo::DrawVideo(VideoClip& clip, Color color)
{
	Vector2Int textureSize = clip.GetTextureSize();

	VideoFrameRenderData* data = GUI::GetResourceById<VideoFrameRenderData>("VideoData");
	if (data == nullptr)
	{
		data = new	VideoFrameRenderData(textureSize);
		GUI::RegisterResource("VideoData", data, [](void* p) {delete (VideoFrameRenderData*)p;  });
		data->countdown = -0.01;

		data->audioSrc = clip.PrepareAudio();
		data->readCrt = clip.LoadFirstFrameAsync(&data->loaded);

		if (data->audioSrc != nullptr)
		{
			AudioManager::Play(*data->audioSrc, 0);
		}
	}
	else
	{
		data->countdown -= 16.6;
	}

	bool frameChanged = false;
	if (data->countdown <= 0)
	{
		if (clip.IsAtEnd())
			return true;

		frameChanged = true;
		data->countdown += clip.GetFrameTime();
	}

	if (data->loaded && !data->decoded)
	{
		Decode(clip, data);
	}

	if (frameChanged)
	{
		data->currentFrame = clip.GetCurrentFrame();

		if (data->readCrt != nullptr)
			data->readCrt->RunAll();

		data->loaded = false;
		data->readCrt = clip.LoadNextFrameAsync(&data->loaded);

		if(!data->decoded)
			Decode(clip, data);

		if (data->texture == nullptr)
			data->texture = GraphicsRenderer::NewTexture(textureSize, false);

		Platform::UpdateTexture(data->texture->GetTextureId(), { {0,0}, textureSize }, { data->pixelData,data->pixelDataSize });

		data->decoded = false;
	}


	GUIImage::DrawSubTexture(*data->texture, { {0,0},clip.GetFrameSize() }, color);

	return false;
}

bool GUIVideo::DrawVideoScaled(VideoClip& clip, Vector2 scale, Color color)
{
	Vector2 size = Vector2(clip.GetFrameSize()) * scale;

	GUI::BeginRelativeLayout({ {0,0},Vector2Int(size) });

	bool done = DrawVideo(clip, color);

	GUI::EndLayout();

	return done;
}