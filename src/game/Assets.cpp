#include "Assets.h"
#include <stdio.h>

SpriteAtlas::SpriteAtlas(int initialSize) {
	sprites.reserve(initialSize);
}

void SpriteAtlas::AddSprite(const Sprite& sprite) {
	sprites.push_back(sprite);
}



bool AudioStream::FillNextBuffer() {

	activeBufferIndex = (activeBufferIndex + 1) % BufferCount;
	Span<uint8_t> buffer = buffers[activeBufferIndex];
	unsigned size = buffer.Size();
	unsigned remaining = GetRemaining();

	if (remaining < size)
		size = remaining;


	int read = fread(buffer.Data(), sizeof(uint8_t), buffer.Size(), stream);
	if (read > 0) {
		activeBufferSize = (unsigned)read;
		streamPos += read;
		return true;
	}
	else {
		activeBufferSize = 0;
		return false;
	}
}

bool AudioStream::Restart() {

	bool success = fseek(stream, 0, SEEK_SET);

	if (success)
		streamPos = 0;

	return success;
}

AudioStream::AudioStream(AudioInfo info, unsigned bufferSize, FILE* stream) {

	this->info = info;
	uint8_t* memory = new uint8_t[bufferSize * BufferCount];
	this->stream = stream;
	for (int i = 0; i < BufferCount; ++i) {
		buffers[i] = { memory + bufferSize * i, bufferSize };
	}
	activeBufferSize = 0;
}

AudioStream::~AudioStream()
{
	delete[] buffers[0].Data();
}

int AudioStream::GetRemaining() const {
	return info.GetTotalSize() - streamPos;
}

void AnimationClip::AddSpritesFromAtlas(const SpriteAtlas* atlas, int start, int count, Vector2Int offset)
{
	for (int i = 0; i < count; ++i) {
		AddFrame({ atlas->GetSprite(start + i), offset });
	}
}
