#include "BinaryData.h"
#include "../Data/GameDatabase.h"
#include "../Debug.h"
#include "../Coroutine.h"

template <class TDataType>
static void LoadData(uint32_t size, FILE* f, std::vector<TDataType>& outData)
{

	outData.clear();
	outData.resize(size);
	fread(outData.data(), sizeof(TDataType), size, f);
}


class BinaryDataLoadDatabaseCrt : public CoroutineImpl {

public:
	FILE* f = nullptr;
	GameDatabase* db = nullptr;
	GameDatabase** store = nullptr;
	BinaryDataLoadDatabaseCrt(FILE* f) : f(f) {}
private:

	uint32_t headerCount = 0;
	DataSectionHeader* headers = nullptr;
	int i = 0;

	CRT_START()
	{
		*store = new GameDatabase();
		db = *store;

		headerCount = 0;
		fread(&headerCount, sizeof(headerCount), 1, f);

		headers = new DataSectionHeader[headerCount];
		fread(headers, headerCount, sizeof(DataSectionHeader), f);

		CRT_YIELD();

		for (i = 0; i < headerCount; ++i)
		{
			//const DataSectionHeader& header = headers[i];

			fseek(f, headers[i].offset, SEEK_SET);

			CRT_YIELD();

			switch (headers[i].type)
			{
			case DataSectionType::Atlases:
				LoadData(headers[i].size, f, db->AtlasDefs); break;
			case DataSectionType::Images:
				LoadData(headers[i].size, f, db->ImageDefs); break;
			case DataSectionType::Frames:
				LoadData(headers[i].size, f, db->FrameDefs); break;
			case DataSectionType::Sprites:
				LoadData(headers[i].size, f, db->SpriteDefs); break;
			case DataSectionType::AnimClips:
				LoadData(headers[i].size, f, db->AnimClipDefs); break;
			case DataSectionType::AnimInstructions:
				LoadData(headers[i].size, f, db->AnimInstructionDefs); break;
			case DataSectionType::Units:
				LoadData(headers[i].size, f, db->UnitDefs); break;
			case DataSectionType::Abilities:
				LoadData(headers[i].size, f, db->AbilityDefs); break;
			case DataSectionType::Upgrades:
				LoadData(headers[i].size, f, db->UpgradeDefs); break;
			case DataSectionType::Effects:
				break;
			case DataSectionType::Wireframe:
				LoadData(headers[i].size, f, db->WireframeDefs); break;
			case DataSectionType::Weapons:
				LoadData(headers[i].size, f, db->WeaponDefs); break;
			case DataSectionType::SoundSets:
				LoadData(headers[i].size, f, db->SoundSetDefs); break;
				break;
			case DataSectionType::AudioClips:
				LoadData(headers[i].size, f, db->AudioClipDefs); break;
			default:
				EXCEPTION("Unknown data section type %i at position %i", headers[i].type, i);
				break;
			}

			CRT_YIELD();
		}

		delete[] headers;


	}
	CRT_END();
};

Coroutine BinaryDataLoader::LoadDatabaseAsync(FILE* f, GameDatabase** out)
{
	auto c = new BinaryDataLoadDatabaseCrt(f);
	c->store = out;

	Coroutine crt = Coroutine(c);

	return crt;
}
