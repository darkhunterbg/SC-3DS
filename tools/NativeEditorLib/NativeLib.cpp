#include "NativeLib.h"
#include "smacker.h"


void* SmkOpen(const char* path)
{
	void* result = smk_open_file(path, SMK_MODE_MEMORY);
	return result;
}

SmkInfo SmkGetInfo(void* handler)
{
	unsigned long w, h, frames;
	double _frameTimeMs;
	smk_info_video((smk)handler, &w, &h, nullptr);
	smk_info_all((smk)handler, nullptr, &frames, &_frameTimeMs);
	_frameTimeMs /= 1000;
	return { (int)w, (int)h, (int)frames };
}

SmkVideoData SmkGetFrameVideoData(void* handler)
{
	smk video = (smk)handler;

	return  { smk_get_palette(video), smk_get_video(video) };
}

void SmkDecodeFrame(void* handler, uint8_t* pixelData, int texLineSize)
{
	smk video = (smk)handler;
	const uint8_t* pal_data = smk_get_palette(video);
	const uint8_t* image = smk_get_video(video);
	int a = 0;

	unsigned long w, h;
	smk_info_video((smk)handler, &w, &h, nullptr);


	for (int y = 0; y < (int)w; ++y)
	{
		for (int x = 0; x < (int)h; ++x)
		{
			int i = (x + y * texLineSize) << 2;


			int index = image[a++];
			index *= 3;


			if (index == 0)
			{
				pixelData[i++] = 0;
				pixelData[i++] = 0;
				pixelData[i++] = 0;
				pixelData[i++] = 0;
				continue;
			}

			pixelData[i++] = (pal_data[index]);
			pixelData[i++] = (pal_data[index + 1]);
			pixelData[i++] = (pal_data[index + 2]);
			pixelData[i++] = 255;
		}
	}
}

void SmkClose(void* handler)
{
	smk_close((smk)handler);
}

void SmkPlayFirst(void* handler)
{
	smk_enable_video((smk)handler, true);
	smk_first((smk)handler);
}

bool SmkPlayNext(void* handler)
{
	return smk_next((smk)handler) != SMK_DONE;
}
