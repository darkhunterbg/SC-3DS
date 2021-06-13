#include "Assets.h"
#include "Data/AssetDataDefs.h"
#include "Debug.h"
#include "Data/AssetDataDefs.h"

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



Vector2Int Font::MeasureString(const char* text) const
{
	return Platform::MeasureString(*this, text);
}

ImageFrame::ImageFrame(const Texture& texture, const ImageFrameDef& def)
	:texture(&texture), offset(def.offset)
{
	Rectangle16 src = { def.atlasOffset, def.size };
	uv = Platform::GenerateUV(texture.GetTextureId(), src);
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

