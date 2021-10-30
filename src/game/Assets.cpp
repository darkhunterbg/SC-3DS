#include "Assets.h"
#include "Data/AssetDataDefs.h"
#include "Debug.h"
#include "Engine/AssetLoader.h"
#include "Engine/JobSystem.h"

#include "Platform.h"

#include "Loader/smacker.h"

#include "Profiler.h"

#include <stdio.h>
#include <cstring>

Texture::~Texture()
{
	Platform::DestroyTexture(id);
}

unsigned AudioClip::FillNextBuffer(Span<uint8_t> buffer)
{
	if (stream == nullptr) return 0;

	unsigned size = buffer.Size();

	size = std::min(size, info.GetTotalSize() - _streamPos);

	if (size > 0)
	{
		fseek(stream, _streamStartPos + _streamPos, SEEK_SET);
		size = fread(buffer.Data(), sizeof(uint8_t), size, stream);
	}

	_streamPos += size;

	return size;
}
bool AudioClip::Restart()
{
	if (stream == nullptr)
		return true;

	_streamPos = 0;

	bool success = fseek(stream, _streamStartPos, SEEK_SET);
	return success;
}

class AudioClipStreamAudioCrt : public CoroutineRImpl<unsigned> {

	AudioClip* _clip;
	Coroutine _fillCrt = nullptr;
	Span<uint8_t> _buffer;
	int _read = 0;

public:
	AudioClipStreamAudioCrt(AudioClip& clip, Span<uint8_t> buffer) :
		_clip(&clip), _buffer(buffer)
	{
	}

	CRT_START()
	{
		if (_clip->IsAtEnd())
		{
			CRT_RETURN(0);
		}

		_fillCrt = AssetLoader::RunIOAsync([this]() { _read = _clip->FillNextBuffer(_buffer); });

		CRT_WAIT_FOR(_fillCrt);

		CRT_RETURN(_read);
	}
	CRT_END();
};

CoroutineR<unsigned> AudioClip::FillAudioAsync(Span<uint8_t> buffer)
{
	return CoroutineR<unsigned>(new AudioClipStreamAudioCrt(*this, buffer));
}
AudioClip::AudioClip(AudioInfo info, FILE* stream)
{
	this->info = info;
	this->stream = stream;

	fgetpos(stream, &_streamStartPos);
	//fclose(stream);
}

AudioClip::~AudioClip()
{
	//if (stream != nullptr)
	//	fclose(stream);
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


VideoClip::VideoClip(FILE* stream) : _stream(stream)
{
	fgetpos(stream, &_streamStartPos);
}
VideoClip::~VideoClip()
{
	if (_handle)
	{
		if (_loadingCrt) _loadingCrt->RunAll();

		smk_close((smk)_handle);
		_handle = nullptr;

	}
}
void VideoClip::OpenVideo()
{
	fseek(_stream, _streamStartPos, SEEK_SET);
	_handle = smk_open_filepointer(_stream, SMK_MODE_DISK);

	GAME_ASSERT(_handle, "Faield to load video from file pointer");

	smk video = (smk)_handle;

	unsigned char   a_trackmask, a_channels[7], a_depth[7];
	unsigned long   a_rate[7];
	unsigned long	w, h, frames;

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
void VideoClip::Close()
{
	if (_handle)
	{
		if (_loadingCrt) _loadingCrt->RunAll();
		_loadingCrt = nullptr;

		smk_close((smk)_handle);
		_handle = nullptr;
	}

	if (_hasAudio)
	{
		_audioSrc.ClearBuffers();
		_audioData.resize(0);
		_audioData.shrink_to_fit();
	}
}
IAudioSource* VideoClip::PrepareAudio()
{
	Warmup();

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

		smk_enable_audio(video, 0, false);

		smk_first(video);
	}

	return &_audioSrc;
}

Coroutine VideoClip::LoadFirstFrameAsync()
{
	Warmup();

	if (_loadingCrt) _loadingCrt->RunAll();

	smk video = (smk)_handle;

	smk_enable_audio(video, 0, false);
	smk_enable_video(video, true);

	_loadingCrt = AssetLoader::RunIOAsync([video]() {smk_first(video); });
	return _loadingCrt;
}
Coroutine VideoClip::LoadNextFrameAsync()
{
	Warmup();

	if (_loadingCrt) _loadingCrt->RunAll();

	smk video = (smk)_handle;

	_loadingCrt = AssetLoader::RunIOAsync([video]() {smk_next(video); });
	return _loadingCrt;
}


struct DecodeFrameJobData {
	const uint8_t* pal_data;
	const uint8_t* image;
	Vector2Int frameSize;
	uint8_t* pixelData;
	int texLineSize;
};
static DecodeFrameJobData _decodeFrame;


static void Decode(int start, int end)
{
	DecodeFrameJobData data = _decodeFrame;

	for (int y = start; y < end; ++y)
	{
		for (int x = 0; x < data.frameSize.x; ++x)
		{
			int i = (x + y * data.texLineSize) << 2;

			int a = y * data.frameSize.x;

			int index = data.image[y * data.frameSize.x + x];
			index *= 3;

			if (index == 0)
			{
				_decodeFrame.pixelData[i++] = 0;
				_decodeFrame.pixelData[i++] = 0;
				_decodeFrame.pixelData[i++] = 0;
				_decodeFrame.pixelData[i++] = 0;
				continue;
			}

#ifdef _3DS
			_decodeFrame.pixelData[i++] = 255;
			_decodeFrame.pixelData[i++] = (_decodeFrame.pal_data[index + 2]);
			_decodeFrame.pixelData[i++] = (_decodeFrame.pal_data[index + 1]);
			_decodeFrame.pixelData[i++] = (_decodeFrame.pal_data[index]);
#else
			_decodeFrame.pixelData[i++] = (_decodeFrame.pal_data[index]);
			_decodeFrame.pixelData[i++] = (_decodeFrame.pal_data[index + 1]);
			_decodeFrame.pixelData[i++] = (_decodeFrame.pal_data[index + 2]);
			_decodeFrame.pixelData[i++] = 255;
#endif
		}
	}
}


void VideoClip::DecodeCurrentFrame(uint8_t* pixelData, int texLineSize)
{
	smk video = (smk)_handle;
	const uint8_t* pal_data = smk_get_palette(video);
	const uint8_t* image = smk_get_video(video);

	_decodeFrame.pal_data = pal_data;
	_decodeFrame.image = image;
	_decodeFrame.pixelData = pixelData;
	_decodeFrame.texLineSize = texLineSize;
	_decodeFrame.frameSize = GetFrameSize();

	JobSystem::RunJob(GetFrameSize().y, 1, Decode);
}