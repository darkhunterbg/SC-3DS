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
		AddFrame({ atlas->GetSprite(start + i), Vector2Int16(offset) });
	}
}

SpriteFrameAtlas::SpriteFrameAtlas(const SpriteAtlas* atlas)
	:atlas(atlas)
{
	frames.reserve(atlas->GetSprites().Size());
	for (const Sprite& s : atlas->GetSprites()) {
		frames.push_back({ s,{0,0} });
	}
}

void AnimationClip::AddFrameCentered(const SpriteFrame& frame, Vector2Int16 frameSize, bool hFlip) {
	auto& f = frames[frameCount++] = frame;
	f.hFlip = hFlip;
	f.offset -= frameSize / 2;
	if (hFlip)
		f.offset.x = frameSize.x / 2 - frame.offset.x - frame.sprite.rect.size.x;

	this->frameSize = frameSize;
}

UnitAnimationClip::UnitAnimationClip() {
	for (auto& sf : frames) {
		sf.sprite.image = { nullptr };
		sf.shadowSprite.image = { nullptr };
		sf.colorSprite.image = { nullptr };
	}
}
uint8_t UnitAnimationClip::AddFrameCentered(const SpriteFrame& frame, Vector2Int16 frameSize, bool hFlip)
{
	frames[frameCount].sprite = frame.sprite;
	frames[frameCount].hFlip = hFlip;
	Vector2Int16 offset = frame.offset - frameSize / 2;
	if (hFlip)
		offset.x = frameSize.x / 2 - frame.offset.x - frame.sprite.rect.size.x;
	frames[frameCount].offset = offset;

	this->frameSize = frameSize;

	return frameCount++;
}
void UnitAnimationClip::AddShadowFrameCentered(uint8_t index, const SpriteFrame& frame, Vector2Int16 frameSize, Vector2Int16 additionalOffset)
{
	frames[index].shadowSprite = frame.sprite;
	Vector2Int16 offset = frame.offset - frameSize / 2;
	if (frames[index].hFlip)
		offset.x = frameSize.x / 2 - frame.offset.x - frame.sprite.rect.size.x;
	frames[index].shadowOffset = offset +additionalOffset;
}
void UnitAnimationClip::AddColorFrame(uint8_t index, const SpriteFrame& frame)
{
	frames[index].colorSprite = frame.sprite;
}