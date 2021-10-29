#include "GameDatabase.h"
#include "../Engine/AssetLoader.h"
#include "../Entity/AnimationPlayer.h"
#include "../Debug.h"
#include "../Coroutine.h"

const GameDatabase* GameDatabase::instance = nullptr;

GameDatabase::GameDatabase()
{
	instance = this;

}

const Image& GameDatabase::GetImage(const std::string& path) const
{
	auto r = imageNamesMap.find(path);
	if (r == imageNamesMap.end())
		EXCEPTION("Tried to get invalid image path '%s'", path.data());

	return *r->second;
}

const ImageFrame& GameDatabase::GetIcon(unsigned id) const
{
	return commandIcons->GetFrame(id);
}

const UnitDef* GameDatabase::GetUnit(const char* path) const
{
	for (const UnitDef& def : UnitDefs)
		if (std::strncmp(def.Path, path, sizeof(def.Path)) == 0)
			return  &def;

	return nullptr;
}
const UnitDef* GameDatabase::GetUnit(UnitId id) const
{
	auto r = unitMap.find(id);
	if (r == unitMap.end())
		return nullptr;

	return r->second;
}

const AbilityDef* GameDatabase::GetAbility(const char* path) const
{
	for (const AbilityDef& def : AbilityDefs)
		if (std::strncmp(def.Path, path, sizeof(def.Path)) == 0)
			return  &def;

	return nullptr;
}
const AbilityDef* GameDatabase::GetAbility(AbilityId id) const
{
	auto r = abilityMap.find(id);
	if (r == abilityMap.end())
		return nullptr;

	return r->second;
}

const SoundSetDef* GameDatabase::GetSoundSet(const char* path) const
{
	for (const SoundSetDef& def : SoundSetDefs)
		if (std::strncmp(def.Path, path, sizeof(def.Path)) == 0)
			return  &def;

	return nullptr;
}

const RaceDef* GameDatabase::GetRace(RaceType type) const
{
	for (const RaceDef& def : RaceDefs)
		if (def.Type == type)
			return &def;

	return nullptr;
}

class GameDatabaseLoadAsetReferencesCrt : public CoroutineImpl {

	GameDatabase* db;
	std::vector<const Texture*> textures;
	std::string name;
	const  Texture* tex;
	AudioClip* clip;
	VideoClip* videoClip;
	const ImageFrame* f;
	const Image* img;
	int i = 0;

public: GameDatabaseLoadAsetReferencesCrt(GameDatabase* db) : db(db) {}
	  
	  CRT_START()
	  {
		  textures.reserve(db->AtlasDefs.size());

		  for (i = 0; i < db->AtlasDefs.size(); ++i)
		  {
			  name = db->AtlasDefs[i].GetAtlasName();
			  name = "atlases\\" + name;
			  tex = AssetLoader::LoadTexture(name.data());
			  textures.push_back(tex);

			  CRT_YIELD();
		  }

		  db->audioClips.reserve(db->AudioClipDefs.size());

		  for (i = 0; i < db->AudioClipDefs.size(); ++i)
		  {
			  clip = AssetLoader::LoadAudioClip(db->AudioClipDefs[i].path);
			  db->audioClips.push_back(clip);

			  CRT_YIELD();
		  }

		  db->videoClips.reserve(db->VideoClipDefs.size());

		  for (i = 0; i < db->VideoClipDefs.size(); ++i)
		  {
			  videoClip = AssetLoader::LoadVideoClip(db->VideoClipDefs[i].path);
			  db->videoClips.push_back(videoClip);

			  CRT_YIELD();
		  }


		  db->frames.reserve(db->frames.size());
		  for (const ImageFrameDef& frameDef : db->FrameDefs)
		  {
			  tex = textures[frameDef.atlasId];
			  db->frames.push_back(ImageFrame(*tex, frameDef));
		  }

		  CRT_YIELD();

		  db->images.reserve(db->ImageDefs.size());
		  db->imageNamesMap.reserve(db->ImageDefs.size());
		  for (const ImageDef& imageDef : db->ImageDefs)
		  {

			  f = &db->frames[imageDef.frameStart];
			  db->images.push_back(Image(f, imageDef));
			  img = &db->images.back();
			  db->imageNamesMap[img->GetName()] = img;
		  }

		  CRT_YIELD();


		  db->unitMap.reserve(db->UnitDefs.size());
		  for (const auto& def : db->UnitDefs)
		  {
			  db->unitMap[def.Id] = &def;
		  }

		  CRT_YIELD();

		  db->abilityMap.reserve(db->AbilityDefs.size());
		  for (const auto& def : db->AbilityDefs)
		  {
			  db->abilityMap[def.Id] = &def;
		  }

		  CRT_YIELD();

		  db->CreateRaces();

		  db->commandIcons = db->imageNamesMap["unit\\cmdbtns\\cmdicons"];

		  CRT_YIELD();

		  AnimationPlayer::BuildInstructionCache({ db->AnimInstructionDefs.data(),db->AnimInstructionDefs.size() });

		  CRT_YIELD();

		  db->selections.push_back(&db->GetImage("unit\\thingy\\o022"));
		  db->selections.push_back(&db->GetImage("unit\\thingy\\o032"));
		  db->selections.push_back(&db->GetImage("unit\\thingy\\o048"));
		  db->selections.push_back(&db->GetImage("unit\\thingy\\o062"));
		  db->selections.push_back(&db->GetImage("unit\\thingy\\o072"));
		  db->selections.push_back(&db->GetImage("unit\\thingy\\o094"));
		  db->selections.push_back(&db->GetImage("unit\\thingy\\o110"));
		  db->selections.push_back(&db->GetImage("unit\\thingy\\o122"));
		  db->selections.push_back(&db->GetImage("unit\\thingy\\o146"));
		  db->selections.push_back(&db->GetImage("unit\\thingy\\o224"));

		  db->MoveAbility = db->GetAbility("Generic\\Move");
		  db->AttackAbility = db->GetAbility("Generic\\Attack");
		  db->StopAbility = db->GetAbility("Generic\\Stop");
		  db->PatrolAbility = db->GetAbility("Generic\\Patrol");
		  db->HoldPositionAbility = db->GetAbility("Generic\\Hold Position");

	  }
	  CRT_END();
};

Coroutine GameDatabase::LoadAssetReferencesAsync()
{
	return Coroutine(new GameDatabaseLoadAsetReferencesCrt(this));
}

void GameDatabase::ReleaseLoadedAssets()
{
	for (auto* clip : videoClips)
	{
		clip->Close();
	}
}

void GameDatabase::CreateRaces()
{
	RaceDefs.reserve(3);

	// ============= Terran =======================
	{
		RaceDef d = RaceDef();
		d.Name = "Terran";
		d.SupplyNamePlural = "Supplies";
		d.SupplyBuildingNamePlural = "Supply Depots";
		d.Type = RaceType::Terran;
		d.ConsoleSprite = GetImage("game\\tconsole").GetFrame(0);
		d.ConsoleLowerSprite = GetImage("game\\tconsoledn").GetFrame(0);
		d.ConsoleUpperSprite = GetImage("game\\tconsoleup").GetFrame(0);
		d.CommandIcons = &GetImage("unit\\cmdbtns\\tcmdbtns");
		d.SupplyIcon = GetImage("game\\icons\\tsupply").GetFrame(0);
		d.GasIcon = GetImage("game\\icons\\tgas").GetFrame(0);

		RaceDefs.push_back(d);
	}
	// ============= Zerg =========================
	{
		RaceDef d = RaceDef();
		d.Name = "Zerg";
		d.SupplyNamePlural = "Controls";
		d.SupplyBuildingNamePlural = "Overlords";
		d.Type = RaceType::Zerg;
		d.ConsoleSprite = GetImage("game\\zconsole").GetFrame(0);
		d.ConsoleLowerSprite = GetImage("game\\zconsoledn").GetFrame(0);
		d.ConsoleUpperSprite = GetImage("game\\zconsoleup").GetFrame(0);
		d.CommandIcons = &GetImage("unit\\cmdbtns\\zcmdbtns");
		d.SupplyIcon = GetImage("game\\icons\\zsupply").GetFrame(0);
		d.GasIcon = GetImage("game\\icons\\zgas").GetFrame(0);

		RaceDefs.push_back(d);
	}
	// ============= Protoss ======================
	{
		RaceDef d = RaceDef();
		d.Name = "Protoss";
		d.SupplyNamePlural = "Psi";
		d.SupplyBuildingNamePlural = "Pylons";
		d.Type = RaceType::Protoss;
		d.ConsoleSprite = GetImage("game\\pconsole").GetFrame(0);
		d.ConsoleLowerSprite = GetImage("game\\pconsoledn").GetFrame(0);
		d.ConsoleUpperSprite = GetImage("game\\pconsoleup").GetFrame(0);
		d.CommandIcons = &GetImage("unit\\cmdbtns\\pcmdbtns");
		d.SupplyIcon = GetImage("game\\icons\\psupply").GetFrame(0);
		d.GasIcon = GetImage("game\\icons\\pgas").GetFrame(0);

		RaceDefs.push_back(d);
	}
}
