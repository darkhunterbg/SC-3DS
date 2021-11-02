#pragma once

#include <functional>
#include <string>
#include "Scene.h"

class VideoClip;

class VideoPlaybackScene : public Scene {
	VideoClip* _clip;
	std::string _videoPath;
	std::function<void()> _onDoneCallback;
public:
	VideoPlaybackScene(const char* video, std::function<void()> doneCallback);

	void Start() override;
	void Frame(TimeSlice& frameBudget) override;
	void Stop() override;
};