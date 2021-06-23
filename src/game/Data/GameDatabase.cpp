#include "GameDatabase.h"
#include "../Engine/AssetLoader.h"
#include "../Debug.h"
#include "../Entity/AnimationPlayer.h"

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

const SoundSetDef* GameDatabase::GetSoundSet(const char* path) const {
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

void GameDatabase::LoadAssetReferences()
{
	std::vector<const Texture*> textures;

	for (const AtlasDef& atlases : AtlasDefs) {
		std::string name = atlases.GetAtlasName();
		name = "atlases\\" + name;
		auto tex = AssetLoader::LoadTexture(name.data());
		textures.push_back(tex);
	}

	audioClips.reserve(AudioClipDefs.size());
	for (const AudioClipDef& clipDef : AudioClipDefs) {
		AudioClip* clip = AssetLoader::LoadAudioClip(clipDef.path);
		audioClips.push_back(clip);
	}


	frames.reserve(frames.size());
	for (const ImageFrameDef& frameDef : FrameDefs) {
		const Texture* tex = textures[frameDef.atlasId];
		frames.push_back(ImageFrame(*tex, frameDef));
	}

	images.reserve(ImageDefs.size());
	imageNamesMap.reserve(ImageDefs.size());
	for (const ImageDef& imageDef : ImageDefs) {
		
		const ImageFrame* f = &frames[imageDef.frameStart];
		images.push_back(Image(f, imageDef));
		const auto& img = images.back();
		imageNamesMap[img.GetName()] = &img;
	}

	unitMap.reserve(UnitDefs.size());
	for (const auto& def : UnitDefs) {
		unitMap[def.Id] = &def;
	}

	abilityMap.reserve(AbilityDefs.size());
	for (const auto& def : AbilityDefs) {
		abilityMap[def.Id] = &def;
	}

	CreateRaces();

	commandIcons = imageNamesMap["unit\\cmdbtns\\cmdicons"];

	AnimationPlayer::BuildInstructionCache({ AnimInstructionDefs.data(), AnimInstructionDefs.size() });

	selections.push_back(&GetImage("unit\\thingy\\o022"));
	selections.push_back(&GetImage("unit\\thingy\\o032"));
	selections.push_back(&GetImage("unit\\thingy\\o048"));
	selections.push_back(&GetImage("unit\\thingy\\o062"));
	selections.push_back(&GetImage("unit\\thingy\\o072"));
	selections.push_back(&GetImage("unit\\thingy\\o094"));
	selections.push_back(&GetImage("unit\\thingy\\o110"));
	selections.push_back(&GetImage("unit\\thingy\\o122"));
	selections.push_back(&GetImage("unit\\thingy\\o146"));
	selections.push_back(&GetImage("unit\\thingy\\o224"));
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
		d.ConsoleLowerSprite = GetImage("game\\tconsole").GetFrame(0);
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
		d.ConsoleLowerSprite = GetImage("game\\zconsole").GetFrame(0);
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
		d.ConsoleLowerSprite = GetImage("game\\zconsole").GetFrame(0);
		d.ConsoleUpperSprite = GetImage("game\\zconsoleup").GetFrame(0);
		d.CommandIcons = &GetImage("unit\\cmdbtns\\zcmdbtns");
		d.SupplyIcon = GetImage("game\\icons\\zsupply").GetFrame(0);
		d.GasIcon = GetImage("game\\icons\\zgas").GetFrame(0);

		RaceDefs.push_back(d);
	}
}
