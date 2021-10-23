#include "Assets.h"
#include "Data/AssetDataDefs.h"
#include "Debug.h"
#include "Engine/AssetLoader.h"

#include "Platform.h"

#include "Loader/smacker.h"

#include <stdio.h>

bool AudioClip::FillNextBuffer()
{
	if (stream == nullptr) return false;

	int nextBufferIndex = (_activeBufferIndex + 1) % BufferCount;
	Span<uint8_t> buffer = buffers[nextBufferIndex];
	unsigned size = buffer.Size();
	unsigned remaining = GetRemaining();

	if (remaining < size)
		size = remaining;


	int read = fread(buffer.Data(), sizeof(uint8_t), size, stream);
	if (read > 0)
	{
		_nextBufferSize = (unsigned)read;
		return true;
	}
	else
	{
		_nextBufferSize = 0;
		return false;
	}
}
void AudioClip::SwapBuffers()
{
	if (stream == nullptr)
	{
		_activeBufferIndex = 0;
		_activeBufferSize = buffers[_activeBufferIndex].Size();
		_streamPos = _activeBufferSize;

	}
	else
	{
		_activeBufferIndex = (_activeBufferIndex + 1) % BufferCount;
		_activeBufferSize = _nextBufferSize;
		_streamPos += _nextBufferSize;
	}
}

bool AudioClip::Restart()
{
	if (stream == nullptr)
	{
		_streamPos = 0;
		return true;
	}

	bool success = fseek(stream, _streamStartPos, SEEK_SET);

	if (success)
		_streamPos = 0;

	return success;
}

class AudioClipStreamAudioCrt : public CoroutineRImpl<AudioChannelClip> {

	AudioClip* _clip;
	Coroutine _fillCrt = nullptr;
public: AudioClipStreamAudioCrt(AudioClip& clip) : _clip(&clip) {}
	  CRT_START()
	  {
		  if (_clip->IsAtEnd())
		  {
			  CRT_RETURN({});
		  }


		  _fillCrt = AssetLoader::RunIOAsync([this]() { _clip->FillNextBuffer(); });

		  CRT_WAIT_FOR(_fillCrt);

		  _clip->SwapBuffers();


		  CRT_RETURN(AudioChannelClip{ _clip->GetData() });
	  }
	  CRT_END();
};

CoroutineR<AudioChannelClip> AudioClip::GetNextAudioChannelClipAsync()
{
	return CoroutineR<AudioChannelClip>(new AudioClipStreamAudioCrt(*this));
}

AudioClip::AudioClip(AudioInfo info, unsigned bufferSize, FILE* stream)
{
	this->info = info;
	uint8_t* memory = new uint8_t[bufferSize * BufferCount];
	this->stream = stream;

	fgetpos(stream, &_streamStartPos);

	for (int i = 0; i < BufferCount; ++i)
	{
		buffers[i] = { memory + bufferSize * i, bufferSize };
	}
	_activeBufferSize = 0;
}
AudioClip::AudioClip(AudioInfo info, FILE* stream)
{
	this->info = info;
	uint8_t* memory = new uint8_t[info.GetTotalSize()];
	this->stream = nullptr;
	fread(memory, sizeof(uint8_t), info.GetTotalSize(), stream);
	buffers[0] = { memory, (unsigned)info.GetTotalSize() };
	buffers[1] = { memory, 0 };
	_activeBufferIndex = 0;
	_activeBufferSize = buffers[0].Size();
}

AudioClip::~AudioClip()
{
	if (stream != nullptr)
		fclose(stream);

	delete[] buffers[0].Data();
}

int AudioClip::GetRemaining() const
{
	return info.GetTotalSize() - _streamPos;
}


Vector2Int Font::MeasureString(const char* text) const
{
	return Platform::MeasureString(*this, text);
}

ImageFrame::ImageFrame(const Texture& texture, const ImageFrameDef& def)
	:texture(&texture), offset(def.offset), size(def.size)
{
	Rectangle16 src = { def.atlasOffset, def.size };
	uv = Platform::GenerateUV(texture.GetTextureId(), src);
}

Image::Image(const ImageFrame* frameStart, const ImageDef& def)
	: name(def.name),
	size(def.size),
	frameStart(frameStart),
	frameCount(def.frameCount)
	, colorMaskOffset(def.colorMaskOffset)
{

}

const ImageFrame& Image::GetFrame(unsigned index) const
{
	if (index >= frameCount)
		EXCEPTION("Tried to get frame %i out of %i frames in image %s!", index, frameCount, name.data());

	return frameStart[index];
}

const ImageFrame* Image::GetColorMaskFrame(unsigned index) const
{
	if (colorMaskOffset && index + colorMaskOffset >= frameCount)
		return nullptr;

	return frameStart + index + colorMaskOffset;
}

Vector2Int16 Image::GetImageFrameOffset(unsigned frameId, bool hFlip) const
{
	const auto& frame = GetFrame(frameId);

	Vector2Int16 offset = frame.offset - (size >> 1);
	if (hFlip)
		offset.x = (size.x >> 1) - frame.offset.x - frame.size.x;

	return offset;
}
Vector2Int16 Image::GetImageFrameOffset(const ImageFrame& frame, bool hFlip) const
{

	Vector2Int16 offset = frame.offset - (size >> 1);
	if (hFlip)
		offset.x = (size.x >> 1) - frame.offset.x - frame.size.x;

	return offset;
}



VideoClip::VideoClip(void* smkHandle) : _handle(smkHandle)
{
	smk video = (smk)_handle;

	unsigned char   a_trackmask, a_channels[7], a_depth[7];
	unsigned long   a_rate[7];
	static unsigned long w, h, frames;

	smk_info_audio(video, &a_trackmask, a_channels, a_depth, a_rate);
	smk_info_video(video, &w, &h, nullptr);
	smk_info_all(video, nullptr, &frames, &_frameTimeMs);

	_frameTimeMs /= 1000;
	_totalFrames = (long)frames;
	_frameSize = Vector2Int((int)w, (int)h);

	_hasAudio = a_trackmask == 1;

	_textureSize = { 1,1 };

	while (_textureSize.x < _frameSize.x)
	{
		_textureSize.x = _textureSize.x << 1;
	}
	while (_textureSize.y < _frameSize.y)
	{
		_textureSize.y = _textureSize.y << 1;
	}
}
VideoClip::~VideoClip()
{
	smk_close((smk)_handle);
}


bool VideoClip::IsAtEnd() const
{
	return GetCurrentFrame() == _totalFrames - 1;
}

long VideoClip::GetCurrentFrame() const
{
	smk video = (smk)_handle;
	unsigned long frame;
	smk_info_all(video, &frame, nullptr, nullptr);
	return (long)frame;
}

IAudioSource* VideoClip::PrepareAudio()
{
	if (!_hasAudio) return nullptr;

	if (_audioData.size() == 0)
	{
		smk video = (smk)_handle;
		smk_enable_audio(video, 0, true);
		smk_enable_video(video, false);

		smk_first(video);

		for (long i = 0; i < _totalFrames; ++i)
		{
			const uint8_t* audio = smk_get_audio(video, 0);
			unsigned size = smk_get_audio_size(video, 0);
			int offset = _audioData.size();
			_audioData.resize(offset + size);


			memcpy(_audioData.data() + offset, audio, size);
			smk_next(video);
		}

		_audioData.shrink_to_fit();

		_audioSrc.AddBuffer(_audioData.data(), _audioData.size());
	}

	return &_audioSrc;
}

Coroutine VideoClip::LoadFirstFrameAsync(volatile bool* doneCallbackFlag)
{
	smk video = (smk)_handle;
	smk_enable_audio(video, 0, false);
	smk_enable_video(video, true);

	return AssetLoader::RunIOAsync([this, doneCallbackFlag] {
		smk video = (smk)_handle;
		smk_first(video);
		if (doneCallbackFlag != nullptr)
			*doneCallbackFlag = true;
		});;
}

Coroutine VideoClip::LoadNextFrameAsync(volatile bool* doneCallbackFlag)
{
	return AssetLoader::RunIOAsync([this, doneCallbackFlag] {
		smk video = (smk)_handle;
		smk_next(video);
		if (doneCallbackFlag != nullptr)
			*doneCallbackFlag = true;
		});;
}


void VideoClip::DecodeCurrentFrame(uint8_t* pixelData, int texLineSize)
{
	smk video = (smk)_handle;
	const uint8_t* pal_data = smk_get_palette(video);
	const uint8_t* image = smk_get_video(video);
	int a = 0;

	int w = GetFrameSize().x;
	int h = GetFrameSize().y;


	for (int y = 0; y < h; ++y)
	{
		for (int x = 0; x < w; ++x)
		{
			int i = (x + y * texLineSize) << 2;

			int index = image[++a];
			index *= 3;

#ifdef _3DS
			pixelData[i++] = 255;
			pixelData[i++] = (pal_data[index + 2]);
			pixelData[i++] = (pal_data[index]);
			pixelData[i++] = (pal_data[index + 1]);
#else
			pixelData[i++] = (pal_data[index]);
			pixelData[i++] = (pal_data[index + 1]);
			pixelData[i++] = (pal_data[index + 2]);
			pixelData[i++] = 255;
#endif
		}
	}
}