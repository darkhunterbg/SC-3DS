#include "BinaryData.h"
#include "../Data/GameDatabase.h"
#include "../Debug.h"

template <class TDataType>
static void LoadData(uint32_t size, FILE* f, std::vector<TDataType>& outData) {

	outData.clear();
	outData.resize(size);
	fread(outData.data(), sizeof(TDataType), size, f);
}

GameDatabase* BinaryDataLoader::LoadDatabase(FILE* f)
{
	GameDatabase* db = new GameDatabase();

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
			LoadData(header.size, f, db->AtlasDefs); break;
		case DataSectionType::Images:
			LoadData(header.size, f, db->ImageDefs); break;
		case DataSectionType::Frames:
			LoadData(header.size, f, db->FrameDefs); break;
		case DataSectionType::Sprites:
			LoadData(header.size, f, db->SpriteDefs); break;
		case DataSectionType::AnimClips:
			LoadData(header.size, f, db->AnimClipDefs); break;
		case DataSectionType::AnimInstructions:
			LoadData(header.size, f, db->AnimInstructionDefs); break;
		case DataSectionType::Units:
			LoadData(header.size, f, db->UnitDefs); break;
		case DataSectionType::Abilities:
			LoadData(header.size, f, db->AbilityDefs); break;
		case DataSectionType::Upgrades:
		case DataSectionType::Effects:
			break;
		case DataSectionType::Wireframe:
			LoadData(header.size, f, db->WireframeDefs); break;
		default:
			EXCEPTION("Unknown data section type %i at position %i", header.type, i);
			break;
		}
	}

	delete[] headers;

	return db;
}
