#pragma once

#include "MathLib.h"
#include "Span.h"
#include <vector>
#include <stdint.h>
#include <string>
#include <array>

#include "Engine/IAudioSource.h"

enum class AssetType : uint8_t {
	Unknown = 0,
	Texture = 1,
	Font = 2,
	AudioClip = 3,
	VideoClip = 4,
	Database = 5,
};


typedef std::size_t AssetId;

typedef void* TextureId;

struct ImageDef;
struct ImageFrameDef;

union SubImageCoord {

	Vector2 coords[4];

	struct {
		Vector2 topLeft;
		Vector2 topRight;
		Vector2 bottomLeft;
		Vector2 bottomRight;
	} corners;

	inline Vector2 GetSize() const
	{
		return corners.bottomRight - corners.topLeft;
	}
	inline Rectangle16 GetSource(Vector2Int16 texSize) const
	{
		Vector2Int16 pos = Vector2Int16(Vector2(texSize) * corners.topLeft);
		Vector2Int16 size = Vector2Int16(Vector2(texSize) * GetSize());
		return { pos,size };
	}
};

class Texture {
private:
	TextureId id;
	Vector2Int16 size;
	std::string name;
public:
	AssetId Id;

	Texture(const Texture&) = delete;
	~Texture();

	Texture(const std::string& name, Vector2Int16 size, TextureId id) :
		id(id), size(size), name(name)
	{
	}

	inline const std::string& GetName() const
	{
		return name;
	}
	inline const Vector2Int16& GetSize() const { return size; };
	inline TextureId GetTextureId() const { return id; }
};


class Font {
private:
	void* fontId;
	float scale;
public:
	Font(void* fontId, float scale) : fontId(fontId), scale(scale) {}
	Font(const Font&) = delete;
	Font& operator=(const Font&) = delete;

	Vector2Int MeasureString(const char* text) const;

	template <class TFontType>
	TFontType* GetFontId() const { return reinterpret_cast<TFontType*>(fontId); }
	inline float GetScale() const { return scale; }
	inline void* GetFontIdRaw() const { return fontId; }
};

struct ImageFrame {
	const Texture* texture = nullptr;
	Vector2Int16 offset;
	Vector2Int16 size;

	SubImageCoord uv = {  };

	ImageFrame() {}
	ImageFrame(const Texture& texture, const ImageFrameDef& def);

	inline Rectangle GetRect() const
	{
		return { Vector2Int(offset), Vector2Int(size) };
	}
	inline Rectangle16 GetRect16() const
	{
		return { offset, size };
	}

};

class Image {
private:
	std::string name;
	Vector2Int16 size;
	const ImageFrame* frameStart;
	uint16_t frameCount;
	uint16_t colorMaskOffset;
public:
	Image() {}
	Image(const ImageFrame* frameStart, const ImageDef& def);

	inline const std::string GetName() const { return name; }
	inline const Vector2Int16 GetSize() const { return size; }

	const ImageFrame& GetFrame(unsigned index) const;
	const Span<ImageFrame> GetFrames() const { return { frameStart, frameCount }; }

	const ImageFrame* GetColorMaskFrame(unsigned index) const;

	Vector2Int16 GetImageFrameOffset(unsigned frameId, bool hFlip) const;
	Vector2Int16 GetImageFrameOffset(const ImageFrame& frame, bool hFlip) const;
};

struct AudioInfo {
	int channels;
	int sampleRate;
	int sampleChannelSize;
	int samplesCount;

	inline int GetTotalSize() const
	{
		return GetSampleSize() * samplesCount;
	}

	inline int GetSampleSize() const
	{
		return channels * sampleChannelSize;
	}

	inline int GetBytesPerSec() const
	{
		return GetSampleSize() * sampleRate;
	}

	inline float GetDurationSeconds() const
	{
		return GetTotalSize() / (float)GetBytesPerSec();
	}
};

class AudioClip : public IAudioSource {
public:
	virtual unsigned StreamSize() const override
	{
		return (long)info.GetTotalSize();
	}
	float GetDuration() const
	{
		return info.GetDurationSeconds();
	}
	inline bool IsMono() const
	{
		return info.channels == 1;
	}
	CoroutineR<unsigned> FillAudioAsync(Span<uint8_t> buffer, unsigned streamPos) override;

	AudioClip(const AudioClip&) = delete;
	AudioClip& operator=(const AudioClip&) = delete;

	AudioClip(AudioInfo info, FILE* stream);
	virtual ~AudioClip() override;

	unsigned FillNextBuffer(Span<uint8_t> buffer, unsigned streamPos);


private:
	AudioInfo info;
	FILE* stream;
	fpos_t _streamStartPos = 0;
};



class VideoClip {
public:
	AssetId Id;

	VideoClip(const VideoClip&) = delete;
	VideoClip& operator=(const VideoClip&) = delete;

	VideoClip(FILE* stream);
	~VideoClip();

	IAudioSource* PrepareAudio();

	Coroutine LoadFirstFrameAsync();

	Coroutine LoadNextFrameAsync();
	void DecodeCurrentFrame(uint8_t* outPixelData, int texLineSize);


	inline Vector2Int GetTextureSize() const { return _textureSize; }

	bool IsAtEnd() const;

	inline Vector2Int GetFrameSize() const
	{
		return _frameSize;
	}
	inline double GetFrameTime() const
	{
		return _frameTimeMs;
	}
	long GetCurrentFrame() const;
	inline bool HasAudio() const { return _hasAudio; }
	inline void Warmup() { if (!_handle) OpenVideo(); }
	void Close();
private:
	FILE* _stream;
	fpos_t _streamStartPos = 0;
	void* _handle = nullptr;
	long _totalFrames = 0;
	bool _hasAudio = false;
	double _frameTimeMs = 0;
	Vector2Int _frameSize;
	Vector2Int _textureSize;
	BufferedAudioSource _audioSrc;
	std::vector< uint8_t> _audioData;
	Coroutine _loadingCrt = nullptr;

	void OpenVideo();
};