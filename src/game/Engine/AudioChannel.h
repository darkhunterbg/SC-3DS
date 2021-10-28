#pragma once

#include <array>

#include "IAudioSource.h"

typedef int AudioChannelHandle;

class AudioChannelState {
private:
	void StartStreamingNextClip();
	void StopStreaming();

	IAudioSource* stream = nullptr;
	CoroutineR<AudioChannelClip> streamingCrt = nullptr;
	AudioChannelClip playingClip;
public:
	bool mono = false;
	bool playbackCompleted = true;

	int ChannelId = 0;
	float volume = 1.0f;

	AudioChannelHandle handle = -1;
	unsigned bufferSize = 0;


	AudioChannelClip* CurrentClip();

	bool IsStreaming() const;
	bool IsDone() const;
	void ChangeSource(IAudioSource* src);

	inline bool IsValid() const { return handle != -1; }
	const inline IAudioSource* GetStream() const { return stream; }
};
