#pragma once

#include <array>
#include "../Span.h"

#include "../Assets.h"

typedef int AudioChannelHandle;

struct AudioChannelClip {
	Span<uint8_t> data = {};
	unsigned playPos = 0;

	unsigned Remaining() {
		return data.Size() - playPos;
	}

	bool Done() const {
		return playPos >= data.Size();
	}

	const uint8_t* PlayFrom() const {
		return data.Data() + playPos;
	}

	const float PlayedPercentage() const {
		unsigned size = data.Size();
		if (size == 0)
			return 1;

		return playPos / (float)size;
	}
};

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

	AudioStream* stream = nullptr;

	AudioChannelClip* CurrentClip() {
		return queueSize > 0 ? &clipQueue[0] : nullptr;
	}

	void ClearQueue() {
		queueSize = 0;
		playbackCompleted = true;
	}

	bool IsDone() const { return queueSize == 0 && playbackCompleted; }

	bool IsQueueFull() const { return queueSize == QueueSize; }

	bool QueueClip(AudioChannelClip clip) {
		if (queueSize == QueueSize)
			return false;

		clipQueue[queueSize++] = clip;
		return true;
	}
	void DequeueClip() {
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
