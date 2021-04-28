#pragma once

#include "Assets.h"

#include <array>
#include "Span.h"

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

	AudioChannelHandle handle = -1;
	unsigned bufferSize = 0;

	std::array< AudioChannelClip, QueueSize> clipQueue;
	int queueSize = 0;

	//AudioTrack* track = nullptr;

	AudioChannelClip* CurrentClip() {
		return queueSize > 0 ? &clipQueue[0] : nullptr;
	}

	void ClearQueue() {
		queueSize = 0;
	}

	bool QueueClip(AudioChannelClip clip) {
		if (queueSize == QueueSize)
			return false;

		clipQueue[queueSize++] = clip;
		return true;
	}
	void DequeueClip() {
		if (queueSize == 0)
			return;

		--queueSize;
		if (queueSize == 0)
			return;

		auto clip = clipQueue[queueSize];

		clipQueue[queueSize - 1] = clip;
	}
	bool IsValid() const { return handle != -1; }
};

class AudioSystem {

public:
	AudioSystem();
	void PlayClip(AudioClip clip);
	void UpdateAudio();
private:
	AudioChannelState channel;
};