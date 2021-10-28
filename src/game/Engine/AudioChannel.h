#pragma once

#include <array>

#include "IAudioSource.h"

typedef int AudioChannelHandle;

class AudioChannelState {
	struct AudioBuffer {
		Span<uint8_t> buffer;
		unsigned size = 0;
	};

private:
	void StartStreamingNextBuffer();

	IAudioSource* stream = nullptr;
	CoroutineR<unsigned> streamingCrt = nullptr;
	AudioBuffer buffers[2];
	int currentBuffer = 0;
	unsigned bufferPosition = 0;

	const AudioBuffer& CurrentBuffer() const { return  buffers[currentBuffer]; }
	const AudioBuffer& StreamingBuffer() const { return  buffers[(currentBuffer + 1 ) % 2]; }
	AudioBuffer& CurrentBuffer() { return  buffers[currentBuffer]; }
	AudioBuffer& StreamingBuffer() { return  buffers[(currentBuffer + 1) % 2]; }
public:
	bool mono = false;


	int ChannelId = 0;
	float volume = 1.0f;

	AudioChannelHandle handle = -1;
	unsigned bufferSize = 0;

	Span<uint8_t> GetNextAudioData();
	void FinishStreaming();
	void AdvanceStream(unsigned pos);

	bool IsDone() const;
	void ChangeSource(IAudioSource* src);

	unsigned Remaining() const;
	inline bool IsValid() const { return handle != -1; }
	const inline IAudioSource* GetStream() const { return stream; }


	Span<uint8_t> GetBuffer(int i) const { return  buffers[i].buffer; }
	Span<uint8_t> GetActiveBuffer() const { return CurrentBuffer().buffer; }
	int GetActiveBufferIndex() const { return currentBuffer; }

	void InitBuffer(Span<uint8_t> main, Span<uint8_t> secondary);
};
