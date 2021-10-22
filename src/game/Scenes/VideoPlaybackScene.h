#pragma once

#include <functional>

#include "Scene.h"

class VideoPlaybackScene : public Scene {

public:
	void Start() override;
	void Draw() override;
	void Stop() override;
};