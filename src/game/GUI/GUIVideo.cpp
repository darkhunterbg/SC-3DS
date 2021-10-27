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
	VideoClip* clip = nullptr;

	bool IsCoroutineCompleted() const
	{
		return readCrt == nullptr || readCrt->IsCompleted();
	}

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
	clip.DecodeCurrentFrame(data->pixelData, clip.GetTextureSize().x);
	data->decoded = true;
}

bool GUIVideo::DrawVideo(const char* id, VideoClip& clip, bool loop, Color color)
{
	Vector2Int textureSize = clip.GetTextureSize();

	std::string key = id;
	key += ".VideoData";

	VideoFrameRenderData* data = GUI::GetResourceById<VideoFrameRenderData>(key);
	if (data == nullptr)
	{
		data = new	VideoFrameRenderData(textureSize);
		GUI::RegisterResource(key, data, [](void* p) {delete (VideoFrameRenderData*)p;  });

	}

	if (data->clip != &clip)
	{
		data->countdown = -0.001f;
		data->clip = &clip;
		data->audioSrc = clip.PrepareAudio();
		data->readCrt = clip.LoadFirstFrameAsync();
		data->readCrt->Next();

		if (data->audioSrc != nullptr)
			AudioManager::Play(*data->audioSrc, 0);

	}
	else
		data->countdown -= GUI::GetState().VideoPlaybackTickTime;


	bool frameChanged = false;
	bool ended = false;
	if (data->countdown <= 0)
	{
		if (clip.IsAtEnd()) ended = true;

		if (!clip.IsAtEnd() || loop)
		{
			frameChanged = true;
			data->countdown += clip.GetFrameTime();
		}
	}

	if (data->IsCoroutineCompleted() && !data->decoded)
	{
		Decode(clip, data);
	}

	if (data->readCrt != nullptr)
		data->readCrt->Next();

	if (frameChanged)
	{
		if (data->readCrt != nullptr)
			data->readCrt->RunAll();

		data->currentFrame = clip.GetCurrentFrame();

		if (clip.IsAtEnd())
		{
			if (loop)
			{
				data->readCrt = clip.LoadFirstFrameAsync();
				data->readCrt->Next();
			}
		}
		else
		{
			data->readCrt = clip.LoadNextFrameAsync();
			data->readCrt->Next();
		}

		if (!data->decoded)
			Decode(clip, data);

		if (data->texture == nullptr)
			data->texture = GraphicsRenderer::NewTexture(textureSize, false);

		Platform::UpdateTexture(data->texture->GetTextureId(), { {0,0}, textureSize }, { data->pixelData,data->pixelDataSize });

		data->decoded = false;
	}


	GUIImage::DrawSubTexture(*data->texture, { {0,0},clip.GetFrameSize() }, color);


	return ended;
}

bool GUIVideo::DrawVideoScaled(const char* id, VideoClip& clip, Vector2 scale, bool loop, Color color)
{
	Vector2 size = Vector2(clip.GetFrameSize()) * scale;

	GUI::BeginRelativeLayout({ {0,0},Vector2Int(size) });

	bool done = DrawVideo(id, clip, loop, color);

	GUI::EndLayout();

	return done;
}