#include "BinaryData.h"
#include "../Debug.h"

template <class TDataType>
static void LoadData(uint32_t size, FILE* f, std::vector<TDataType>& outData) {

	outData.clear();
	outData.resize(size);
	fread(outData.data(), sizeof(TDataType), size, f);
}

GameDatabase::GameDatabase(FILE* f)
{
	uint32_t headerCount = 0;
	fread(&headerCount, sizeof(headerCount), 1, f);

	DataSectionHeader* headers = new DataSectionHeader[headerCount];
	fread(headers, headerCount, sizeof(DataSectionHeader), f);

	for (int i = 0; i < headerCount; ++i) {
		const DataSectionHeader& header = headers[i];

		fseek(f, header.offset, SEEK_SET);

		switch (header.type)
		{
		case DataSectionType::Atlases:
			LoadData(header.size, f, Atlases); break;
		case DataSectionType::Images:
			LoadData(header.size, f, Images); break;
		case DataSectionType::Frames:
			LoadData(header.size, f, Frames); break;
		case DataSectionType::Sprites:
			LoadData(header.size, f, Sprites); break;
		case DataSectionType::AnimClips:
			LoadData(header.size, f, AnimationClips); break;
		case DataSectionType::AnimInstructions:
			LoadData(header.size, f, AnimationInstructions); break;
		default:
			//EXCEPTION("Unknown data section type %i at position,", header.type, i);
			break;
		}
	}

	delete[] headers;
}
