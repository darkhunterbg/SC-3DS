#pragma once

#include <array>

#include "IAudioSource.h"

typedef int AudioChannelHandle;

struct AudioChannelState {

	static constexpr const int QueueSize = 2;

	bool mono = false;
	bool playbackCompleted = true;

	int ChannelId = 0;
	float volume = 1.0f;

	AudioChannelHandle handle = -1;
	unsigned bufferSize = 0;

	std::array< AudioChannelClip, QueueSize> clipQueue;
	int queueSize = 0;

	IAudioSource* stream = nullptr;
	CoroutineR<AudioChannelClip> streamingCrt = nullptr;



	AudioChannelClip* CurrentClip()
	{
		return queueSize > 0 ? &clipQueue[0] : nullptr;
	}

	void ClearQueue()
	{
		queueSize = 0;
		playbackCompleted = true;
	}

	bool IsStreaming() const
	{
		return streamingCrt != nullptr;
	}

	void StopStreaming()
	{
		streamingCrt = nullptr;
	}

	bool IsDone() const { return queueSize == 0 && playbackCompleted; }

	bool IsQueueFull() const { return queueSize == QueueSize; }

	bool QueueClip(AudioChannelClip clip)
	{
		if (queueSize == QueueSize)
			return false;

		clipQueue[queueSize++] = clip;
		return true;
	}
	void DequeueClip()
	{
		playbackCompleted = true;
		if (queueSize == 0)
			return;

		--queueSize;
		if (queueSize == 0)
			return;

		const auto& clip = clipQueue[queueSize];

		clipQueue[queueSize - 1] = clip;
		playbackCompleted = false;
	}
	bool IsValid() const { return handle != -1; }
};
