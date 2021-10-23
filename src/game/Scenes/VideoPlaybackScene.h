#pragma once

#include <functional>

#include "Scene.h"

class VideoClip;

class VideoPlaybackScene : public Scene {
	VideoClip* _clip;
public:
	void Start() override;
	void Draw() override;
	void Stop() override;
};