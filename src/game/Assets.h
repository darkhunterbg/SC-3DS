#pragma once

#include "MathLib.h"
#include "Span.h"
#include <vector>
#include <stdint.h>
#include <string>
#include <array>

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

	inline Vector2 GetSize() const {
		return corners.bottomRight - corners.topLeft;
	}
	inline Rectangle16 GetSource(Vector2Int16 texSize) const {
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
	Texture(const Texture&) = delete;


	Texture(const std::string& name, Vector2Int16 size, TextureId id) :
		id(id), size(size), name(name) {}

	inline const std::string& GetName() const {
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

	inline Rectangle GetRect() const {
		return { Vector2Int(offset), Vector2Int(size) };
	}
	inline Rectangle16 GetRect16() const {
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

	inline int GetTotalSize() const {
		return GetSampleSize() * samplesCount;
	}

	inline int GetSampleSize() const {
		return channels * sampleChannelSize;
	}

	inline int GetBytesPerSec() const {
		return GetSampleSize() * sampleRate;
	}

	inline float GetDurationSeconds() const {
		return GetTotalSize() / (float)GetBytesPerSec();
	}
};

class AudioClip {
public:
	Span<uint8_t> GetData() const {
		return { buffers[_activeBufferIndex].Data(), (unsigned)_activeBufferSize };
	}
	int GetSize() const {
		return info.GetTotalSize();
	}
	float GetDuration() const {
		return info.GetDurationSeconds();
	}
	bool IsAtEnd() const {
		return GetRemaining() == 0;
	}

	int GetRemaining() const;
	bool FillNextBuffer();
	void SwapBuffers();
	bool Restart();

	AudioClip(const AudioClip&) = delete;
	AudioClip& operator=(const AudioClip&) = delete;

	/// <summary>
	/// Create unbuffered clip (all data will be loaded in memory)
	/// </summary>
	AudioClip(AudioInfo info, FILE* stream);
	/// <summary>
	/// Create buffered clip (data will be streamed between 2 buffers)
	/// </summary>
	AudioClip(AudioInfo info, unsigned bufferSize, FILE* stream);
	~AudioClip();

	uint16_t id = 0;
private:
	AudioInfo info;

	static constexpr const int BufferCount = 2;

	std::array< Span<uint8_t>, BufferCount> buffers;
	int _activeBufferSize = 0;
	int _nextBufferSize = 0;
	FILE* stream;
	int _activeBufferIndex = 1;
	int _streamPos = 0;
	fpos_t _streamStartPos = 0;
};