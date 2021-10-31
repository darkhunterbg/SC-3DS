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

void BufferedAudioSource::ClearBuffers()
{
	for (const auto& b : _buffers)
	{
		if (b.owned)
			delete[] b.data;
	}
}

CoroutineR<unsigned> BufferedAudioSource::FillAudioAsync(Span<uint8_t> buffer, unsigned streamPos)
{
	return CoroutineFromFunctionResult<unsigned>([this, buffer, streamPos]() {
		return	this->FillNext(buffer, streamPos);
		});
}

unsigned BufferedAudioSource::FillNext(Span<uint8_t> buffer, unsigned streamPos)
{
	int remaining = buffer.Size();

	if (remaining == 0)return 0;

	int _currentBuffer = 0;
	while (streamPos > 0 && streamPos > _buffers[_currentBuffer].size)
	{
		streamPos -= _buffers[_currentBuffer].size;
		++_currentBuffer;
	}

	_buffers[_currentBuffer].position = (int)streamPos;

	while (remaining > 0 && _currentBuffer< _buffers.size())
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
			memcpy(buffer.Data() + offset, b.data + b.position, read);
			remaining -= read;
			b.position += read;
		}
	}

	return buffer.Size() - remaining;
}


unsigned BufferedAudioSource::StreamSize() const
{
	long size = 0;
	for (auto& buffer : _buffers)
		size += buffer.size;

	return size;
}

BufferedAudioSource::~BufferedAudioSource()
{
	ClearBuffers();
}
