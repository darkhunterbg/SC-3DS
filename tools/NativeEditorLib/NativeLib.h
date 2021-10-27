#pragma once
#define EXPORT __declspec(dllexport)

#include "pch.h"
#include <inttypes.h>

extern "C" {
	struct SmkInfo {
		int width, height, frameCount;
	};

	struct SmkVideoData {
		const uint8_t* pal_data;
		const uint8_t* image;
	};

	EXPORT void* SmkOpen(const char* path);
	EXPORT void SmkClose(void* handler);
	EXPORT SmkInfo SmkGetInfo(void* handler);
	EXPORT void SmkDecodeFrame(void* handler, uint8_t* pixelData, int texLineSize);
	EXPORT void SmkPlayFirst(void* handler);
	EXPORT bool SmkPlayNext(void* handler);
	EXPORT SmkVideoData SmkGetFrameVideoData(void* handler);
}