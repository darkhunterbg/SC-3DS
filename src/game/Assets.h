#pragma once

#include "MathLib.h"
#include "Span.h"
#include <vector>
#include <stdint.h>
#include <string>
#include <array>

#include "Engine/GraphicsPrimitives.h"


class SpriteAtlas {
private:
	std::vector<Sprite> sprites;
public:
	inline const Span<Sprite> GetSprites() const { return { sprites.data(),sprites.size() }; }
	inline const Sprite& GetSprite(int pos) const { return sprites[pos]; };

	SpriteAtlas(int initialSize = 0);

	void AddSprite(const Sprite& sprite);
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
struct AudioClip {

	AudioInfo info;
	uint8_t* data;
	uint16_t id = 0;

	Span< uint8_t> GetData() const {
		return { data, (unsigned)info.GetTotalSize() };
	}
	unsigned GetSize() const {
		return (unsigned)info.GetTotalSize();
	}
	float GetDuration() const {
		return  info.GetDurationSeconds();
	}


};
class AudioStream {
public:
	Span<uint8_t> GetData() const {
		return { buffers[activeBufferIndex].Data(), (unsigned)activeBufferSize };
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
	bool Restart();

	AudioStream(AudioInfo info, unsigned bufferSize, FILE* stream);
	~AudioStream();
private:
	AudioInfo info;

	static constexpr const int BufferCount = 2;

	std::array< Span<uint8_t>, BufferCount> buffers;
	int activeBufferSize = 0;
	FILE* stream;
	int activeBufferIndex = 1;
	int streamPos = 0;
};


struct SpriteFrame {
	Sprite sprite;
	Vector2Int16 offset;
	bool hFlip = false;
};
class SpriteFrameAtlas {
private:
	const SpriteAtlas* atlas;
	std::vector<SpriteFrame> frames;
public:
	Vector2Int FrameSize;
	SpriteFrameAtlas(const SpriteAtlas* atlas);
	inline void SetOffset(int index, Vector2Int offset) {
		frames[index].offset = Vector2Int16(offset);
	}
	inline const SpriteFrame& GetFrame(int index) const {
		return frames[index];
	}
};

class AnimationClip {

public:
	uint8_t frameTime = 1;
	bool looping = false;
private:
	uint8_t frameCount = 0;
	std::array< SpriteFrame, 16> frames;
	Vector2Int16 frameSize;
public:
	
	AnimationClip() {}
	AnimationClip(const AnimationClip&) = delete;
	AnimationClip& operator=(const AnimationClip&) = delete;

	void AddSpritesFromAtlas(const SpriteAtlas* atlas, int start, int count, Vector2Int offset = { 0,0 });

	inline Vector2Int16 GetFrameSize() const { return frameSize; }
	inline void AddFrame(const SpriteFrame& frame) {
		frames[frameCount++] = frame;
	}
	void AddFrameCentered(const SpriteFrame& frame, Vector2Int16 frameSize, bool hFlip = false);
	inline void SetFrameOffset(uint8_t frame, Vector2Int16 offset) {
		frames[frame].offset = offset;
	}
	inline uint16_t GetDuration() const { return frameCount * (uint16_t)frameTime; }
	inline Span<SpriteFrame> GetFrames() const {
		return { frames.data(), frameCount };
	}
	inline uint8_t GetFrameCount() const { return frameCount; }
	inline const SpriteFrame& GetFrame(uint8_t index) const {
		return frames[index];
	}
};

struct UnitSpriteFrame {
	Sprite sprite;
	Vector2Int16 offset;
	Sprite shadowSprite;
	Vector2Int16 shadowOffset;
	Sprite colorSprite;
	bool hFlip = false;
};

class UnitAnimationClip {

public:
	uint8_t frameTime = 1;
	bool looping = false;
private:
	uint8_t frameCount = 0;
	std::array<UnitSpriteFrame, 16> frames;
	Vector2Int16 frameSize;
public:
	
	UnitAnimationClip();
	UnitAnimationClip(const UnitAnimationClip&) = delete;
	UnitAnimationClip& operator=(const UnitAnimationClip&) = delete;

	inline Vector2Int16 GetFrameSize() const { return frameSize; }
	inline uint16_t GetDuration() const { return frameCount * (uint16_t)frameTime; }
	inline const Span<UnitSpriteFrame> GetFrames() const {
		return { frames.data(), frameCount };
	}
	inline uint8_t GetFrameCount() const { return frameCount; }
	inline const UnitSpriteFrame& GetFrame(uint8_t index) const {
		return frames[index];
	}

	uint8_t AddFrameCentered(const SpriteFrame& frame, Vector2Int16 frameSize, bool hFlip = false);
	void AddShadowFrameCentered(uint8_t frameIndex, const SpriteFrame& frame, Vector2Int16 frameSize, Vector2Int16 additionalOffset = { 0,0 });
	void AddColorFrame(uint8_t frameIndex, const SpriteFrame& frame);

};
