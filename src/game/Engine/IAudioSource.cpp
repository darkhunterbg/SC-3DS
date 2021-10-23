#include "IAudioSource.h"

#include <cstring>

void BufferedAudioSource::AddCopyBuffer(const uint8_t* _buffer, unsigned size)
{
	uint8_t* buffer = new uint8_t[size];
	memcpy(buffer, _buffer, size);
	AddAndOwnBuffer(buffer, size);
}

void BufferedAudioSource::AddAndOwnBuffer(uint8_t* _buffer, unsigned size)
{
	_buffers.push_back({ _buffer, size , true });
}
void BufferedAudioSource::AddBuffer(uint8_t* _buffer, unsigned size)
{
	_buffers.push_back({ _buffer, size , false });
}

CoroutineR<AudioChannelClip> BufferedAudioSource::GetNextAudioChannelClipAsync()
{
	return CoroutineFromFunctionResult<AudioChannelClip>([this]() {
		if (IsAtEnd()) return AudioChannelClip();
		Buffer b = _buffers[_nextBuffer++];
		return AudioChannelClip{ {b.data , b.size } };
		});
}

bool BufferedAudioSource::Restart()
{
	_nextBuffer = 0;
	return true;
}

bool BufferedAudioSource::IsAtEnd() const
{
	return _nextBuffer >= _buffers.size();;
}

BufferedAudioSource::~BufferedAudioSource()
{
	for (const auto& b : _buffers)
	{
		if (b.owned)
			delete[] b.data;
	}
}
