#pragma once

#include "../Span.h"
#include "../Coroutine.h"

#include <inttypes.h>

struct AudioChannelClip {
	Span<uint8_t> data = {};
	unsigned playPos = 0;

	unsigned Remaining()
	{
		return data.Size() - playPos;
	}

	bool IsEmpty() const { return data.Size() == 0; }

	bool Done() const
	{
		return playPos >= data.Size();
	}

	const uint8_t* PlayFrom() const
	{
		return data.Data() + playPos;
	}

	const float PlayedPercentage() const
	{
		unsigned size = data.Size();
		if (size == 0)
			return 1;

		return playPos / (float)size;
	}
};


class IAudioSource {
public:
	virtual CoroutineR<AudioChannelClip> GetNextAudioChannelClipAsync() = 0;
	virtual bool Restart() = 0;
	virtual bool IsAtEnd() const = 0;
	virtual ~IAudioSource() {}
};