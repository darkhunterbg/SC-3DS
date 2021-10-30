#pragma once


#include "GUIBase.h"

class GUIVideo {
private:
	GUIVideo() = delete;
	~GUIVideo() = delete;
public:
	static void RestartNextVideo();

	// Reutrn TRUE when playback is done
	static bool DrawVideo(const char* id, VideoClip& clip, bool loop = false, Color color = Colors::White);
	static bool DrawVideoScaled(const char* id, VideoClip& clip, Vector2 scale,  bool loop = false, Color color = Colors::White);
};