#pragma once

#include "MathLib.h"
#include "Span.h"
#include <vector>
#include <stdint.h>
#include <string>
#include <array>

typedef void* Texture;



struct Sprite {
	Texture textureId;
	Rectangle rect;

	template <class TTextureType>
	TTextureType* GetTextureId() const { return reinterpret_cast<TTextureType*>(textureId); }
};

class SpriteAtlas {
private:
	std::vector<Sprite> sprites;
public:
	inline Span<Sprite> GetSprites() const { return { sprites.data(),sprites.size() }; }
	inline const Sprite& GetSprite(int pos) const { return sprites[pos]; };

	SpriteAtlas(int initialSize = 0);

	void AddSprite(const Sprite& sprite);
};


struct Font {
	void* fontId;

	template <class TFontType>
	TFontType* GetFontId() const { return reinterpret_cast<TFontType*>(fontId); }

	inline bool IsValid() { return fontId != nullptr; }
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