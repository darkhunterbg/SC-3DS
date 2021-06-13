#pragma once

#include <stdint.h>
#include <string.h>
#include "../Assets.h"
#include <stdio.h>

#pragma pack(push,1)
struct  WaveHeader
{
	/* RIFF Chunk Descriptor */
	char         RIFF[4];        // RIFF Header Magic header
	uint32_t        ChunkSize;      // RIFF Chunk Size
	char         WAVE[4];        // WAVE Header
	/* "fmt" sub-chunk */
	char         fmt[4];         // FMT header
	uint32_t        Subchunk1Size;  // Size of the fmt chunk
	uint16_t        AudioFormat;    // Audio format 1=PCM,6=mulaw,7=alaw,     257=IBM Mu-Law, 258=IBM A-Law, 259=ADPCM
	uint16_t        NumOfChan;      // Number of channels 1=Mono 2=Sterio
	uint32_t        SamplesPerSec;  // Sampling Frequency in Hz
	uint32_t        bytesPerSec;    // bytes per second
	uint16_t        blockAlign;     // 2=16-bit mono, 4=16-bit stereo
	uint16_t        bitsPerSample;  // Number of bits per sample
	/* "data" sub-chunk */

};

struct RiffSubchunkHeader {
	char SubchunkID[4];
	uint32_t  SubchunkSize;
};
#pragma pack(pop)

class WaveLoader {
private:
	WaveLoader() = delete;
	~WaveLoader() = delete;
public:
	static bool ReadWAVHeader(FILE* file, AudioInfo* result);

	static bool LoadWAVData(FILE* file, Span<uint8_t> buffer, const AudioInfo& header);

};