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

CoroutineR<unsigned> BufferedAudioSource::FillAudioAsync(Span<uint8_t> buffer)
{
	return CoroutineFromFunctionResult<unsigned>([this, buffer]() {
		return	this->FillNext(buffer);
		});
}

unsigned BufferedAudioSource::FillNext(Span<uint8_t> buffer)
{
	int remaining = buffer.Size();

	while (remaining > 0 && !IsAtEnd())
	{
		Buffer& b = _buffers[_currentBuffer];
		if ((unsigned)b.position >= b.size)
		{
			++_currentBuffer;
		}
		else
		{
			int offset = buffer.Size() - remaining;
			int read = std::min(remaining, (int)b.size - b.position);
			memcpy(buffer.Data() + offset, b.data, read);
			remaining -= read;
			b.position += read;
		}
	}

	return buffer.Size() - remaining;
}

bool BufferedAudioSource::Restart()
{
	_currentBuffer = 0;
	if (_buffers.size() > 0)
		_buffers[_currentBuffer].position = 0;
	return true;
}

bool BufferedAudioSource::IsAtEnd() const
{
	return _currentBuffer >= _buffers.size();
}

BufferedAudioSource::~BufferedAudioSource()
{
	for (const auto& b : _buffers)
	{
		if (b.owned)
			delete[] b.data;
	}
}
