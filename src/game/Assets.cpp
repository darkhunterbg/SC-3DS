#include "Assets.h"
#include "Data/AssetDataDefs.h"
#include "Debug.h"

#include "Platform.h"
#include <stdio.h>

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
		sf.sprite.textureId = nullptr;
		sf.shadowSprite.textureId = nullptr;
		sf.colorSprite.textureId = nullptr;
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
	frames[index].shadowOffset = offset + additionalOffset;
}
void UnitAnimationClip::AddColorFrame(uint8_t index, const SpriteFrame& frame)
{
	frames[index].colorSprite = frame.sprite;
}

Vector2Int Font::MeasureString(const char* text) const
{
	return Platform::MeasureString(*this, text);
}

ImageFrame::ImageFrame(const Texture& texture, const ImageFrameDef& def)
	:texture(&texture), offset(def.offset), size(def.size)
{
	uv[0] = Vector2(def.atlasOffset) / Vector2(texture.GetSize());
	uv[1] = Vector2(def.atlasOffset + size) / Vector2(texture.GetSize());
}


Image::Image(const ImageFrame* frameStart, const ImageDef& def)
	: name(def.name),
	size(def.size),
	frameStart(frameStart),
	frameCount(def.frameCount)
{

}

const ImageFrame& Image::GetFrame(unsigned index) const
{
	if (index >= frameCount)
		EXCEPTION("Tried to get frame %i out of %i frames in image %s!", index, frameCount, name.data());

	return frameStart[index];
}

