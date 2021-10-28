#pragma once

#include "../Span.h"
#include "../Coroutine.h"

#include <vector>
#include <inttypes.h>

struct AudioChannelClip {
	Span<uint8_t> data = {};
	unsigned playPos = 0;

	unsigned Remaining()
	{
		return data.Size() - playPos;
	}

	bool IsEmpty() const { return data.Size() == 0; }

	bool IsDone() const
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
	virtual CoroutineR<unsigned> FillAudioAsync(Span<uint8_t> buffer) = 0;
	virtual bool Restart() = 0;
	virtual bool IsAtEnd() const = 0;
	virtual ~IAudioSource() {}
};

class BufferedAudioSource : public IAudioSource {
	struct Buffer {
		uint8_t* data;
		unsigned size;
		bool owned = false;
		int position = 0;
	};
private:
	std::vector<Buffer> _buffers;
	int _currentBuffer = 0;

public:
	void AddCopyBuffer(const uint8_t* _buffer, unsigned size);
	void AddAndOwnBuffer(uint8_t* _buffer, unsigned size);
	void AddBuffer(uint8_t* _buffer, unsigned size);
	virtual CoroutineR<unsigned> FillAudioAsync(Span<uint8_t> buffer) override;
	unsigned FillNext(Span<uint8_t> buffer);
	virtual bool Restart() override;
	virtual bool IsAtEnd() const override;
	virtual ~BufferedAudioSource() override;
};