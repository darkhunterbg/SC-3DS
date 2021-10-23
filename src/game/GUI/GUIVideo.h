#pragma once


#include "GUIBase.h"

class GUIVideo {
private:
	GUIVideo() = delete;
	~GUIVideo() = delete;
public:
	// Reutrn TRUE when playback is done
	static bool DrawVideo(VideoClip& clip, Color color = Colors::White);

	static bool DrawVideoScaled(VideoClip& clip, Vector2 scale, Color color = Colors::White);
};