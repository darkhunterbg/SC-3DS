#include "Wave.h"


bool WaveLoader::ReadWAVHeader(FILE* file, AudioInfo* result) {
	WaveHeader header;

	if (!fread(&header, sizeof(WaveHeader), 1, file))
		return false;

	if (strncmp(header.RIFF, "RIFF", 4) != 0 ||
		strncmp(header.WAVE, "WAVE", 4) != 0 ||
		strncmp(header.fmt, "fmt ", 4) != 0)
		return false;

	if (header.Subchunk1Size != 16 || header.AudioFormat != 1) {
		// Unsupported WAV format
		return false;
	}

	RiffSubchunkHeader subHeader;

	for (int i = 0; i < 5; ++i) {
		if (!fread(&subHeader, sizeof(RiffSubchunkHeader),1, file) )
			return false;

		if (strncmp(subHeader.SubchunkID, "data", 4) != 0) {
			if (fseek(file, subHeader.SubchunkSize, SEEK_CUR))
				return false;
		}
		else {
			break;
		}
	}

	if (subHeader.SubchunkSize == 0)
		return false;


	result->channels = header.NumOfChan;
	result->sampleRate = header.SamplesPerSec;
	result->sampleChannelSize = header.bitsPerSample / 8;
	result->samplesCount = subHeader.SubchunkSize / result->GetSampleSize();

	return true;
}

bool WaveLoader::LoadWAVData(FILE* file, Span<uint8_t> buffer, const AudioInfo& header) {

	size_t size = fread(buffer.Data(), sizeof(uint8_t), buffer.Size(), file);
	return size == header.GetTotalSize();


}
