#include "AudioChannel.h"
#include "../Debug.h"

#include <cstring>

Span<uint8_t> AudioChannelState::GetNextAudioData()
{
	if (IsDone()) return {};

	if (streamingCrt != nullptr)
		streamingCrt->Next();

	if (Remaining() == 0)
	{
		FinishStreaming();

		currentBuffer = (currentBuffer + 1) % 2;
		bufferPosition = 0;

		streamingCrt = nullptr;

		if (IsDone()) return {};

		StartStreamingNextBuffer();
	}

	Span<uint8_t> audio = { CurrentBuffer().buffer.Data() + bufferPosition, Remaining() };
	return audio;
}

void AudioChannelState::FinishStreaming()
{
	if (!streamingCrt) return;

	auto& streamingBuffer = StreamingBuffer();

	streamingCrt->RunAll();
	streamingBuffer.size = streamingCrt->GetResult();
	_streamPosition += streamingCrt->GetResult();
	streamingCrt = nullptr;



	if (streamingBuffer.size < streamingBuffer.buffer.Size())
	{
		memset(streamingBuffer.buffer.Data(), 0, streamingBuffer.buffer.Size() - streamingBuffer.size);
	}
}

void AudioChannelState::AdvanceStream(unsigned pos)
{
	bufferPosition += pos;
}

void AudioChannelState::StartStreamingNextBuffer()
{
	if (stream != nullptr)
	{
		// Workaround for destroying coroutines which are running on IO thread
		if (streamingCrt && !streamingCrt->IsCompleted())
			streamingCrt->RunAll();

		auto& buffer = StreamingBuffer();
		streamingCrt = stream->FillAudioAsync(buffer.buffer, _streamPosition);
		streamingCrt->Next();
	}
}

bool AudioChannelState::IsDone()  const
{
	if (!_enabled) return true;

	return stream == nullptr ||
		(Remaining() == 0 &&
			streamingCrt == nullptr);
}

void AudioChannelState::ChangeSource(IAudioSource* src, unsigned streamPosition)
{
	GAME_ASSERT(CurrentBuffer().buffer.Size() != 0, "AudioChannel %i (handle: %i) has no buffers!", ChannelId, handle);

	// WARNING: this is not thread-safe, audio thread might be reading this stream, use some kind of mutex

	_streamPosition = streamPosition;
	stream = src;
	bufferPosition = 0;
	CurrentBuffer().size = 0;
	if (stream)
		StartStreamingNextBuffer();
}

unsigned AudioChannelState::Remaining() const
{
	auto& buffer = CurrentBuffer();

	if (buffer.size > bufferPosition)
		return buffer.size - bufferPosition;

	return 0;
}

void AudioChannelState::InitBuffer(Span<uint8_t> main, Span<uint8_t> secondary)
{
	buffers[0].buffer = main;
	buffers[1].buffer = secondary;
}
