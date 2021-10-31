#pragma once

#include "../Span.h"
#include "../Coroutine.h"

#include <vector>
#include <inttypes.h>


class IAudioSource {
public:
	virtual CoroutineR<unsigned> FillAudioAsync(Span<uint8_t> buffer, unsigned streamStartPos) = 0;
	virtual unsigned StreamSize() const = 0;
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
public:
	void AddCopyBuffer(const uint8_t* _buffer, unsigned size);
	void AddAndOwnBuffer(uint8_t* _buffer, unsigned size);
	void AddBuffer(uint8_t* _buffer, unsigned size);
	void ClearBuffers();
	virtual CoroutineR<unsigned> FillAudioAsync(Span<uint8_t> buffer,unsigned streamPos) override;
	unsigned FillNext(Span<uint8_t> buffer, unsigned streamPos);
	virtual unsigned StreamSize() const override;
	virtual ~BufferedAudioSource() override;
};