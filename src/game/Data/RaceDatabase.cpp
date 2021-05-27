#include "RaceDatabase.h"
#include "Generated.h"

RaceDef RaceDatabase::Terran;
RaceDef RaceDatabase::Zerg;
RaceDef RaceDatabase::Protoss;

void RaceDatabase::Init()
{
	// ============= Terran =======================
	{
		RaceDef& d = Terran;
		d.Name = "Terran";
		d.SupplyNamePlural = "Supplies";
		d.Type = RaceType::Terran;
		d.ConsoleSprite.Path = "game_tconsole.t3x";
		d.SupplyIconId = 5;
		d.GasIconId = 2;
		d.AdvisorErrorSounds.SoundPath = "sound/terran/advisor/taderr";
		d.AdvisorErrorSounds.SoundIds = { 0,1,2,3,4,6 };
		d.AdvisorUpdateSounds.SoundPath = "sound/terran/advisor/tadupd";
		d.AdvisorUpdateSounds.SoundIds = { 0,1,2,3,4,5,6,7 };
		d.GameMusic.push_back({ "music/terran1.wav" });
		d.GameMusic.push_back({ "music/terran2.wav" });
		d.GameMusic.push_back({ "music/terran3.wav" });
		d.CommandIconsLoad = []() { Terran.CommandIconsAtlas = SpriteDatabase::Load_unit_cmdbtns_tcmdbtns(); };
	}
	// ============= Zerg =========================
	{
		RaceDef& d = Zerg;
		d.Name = "Zerg";
		d.SupplyNamePlural = "Controls";
		d.Type = RaceType::Zerg;
		d.ConsoleSprite.Path = "game_zconsole.t3x";
		d.SupplyIconId = 4;
		d.GasIconId = 1;
		d.AdvisorErrorSounds.SoundPath = "sound/zerg/advisor/zaderr";
		d.AdvisorErrorSounds.SoundIds = { 0,1,2,6 };
		d.AdvisorUpdateSounds.SoundPath = "sound/zerg/advisor/zadupd";
		d.AdvisorUpdateSounds.SoundIds = { 0,1,2,4 };
		d.GameMusic.push_back({ "music/zerg1.wav" });
		d.GameMusic.push_back({ "music/zerg2.wav" });
		d.GameMusic.push_back({ "music/zerg3.wav" });
		d.CommandIconsLoad = []() { Zerg.CommandIconsAtlas = SpriteDatabase::Load_unit_cmdbtns_zcmdbtns(); };
	}
	// ============= Protoss ======================
	{
		RaceDef& d = Protoss;
		d.Name = "Protoss";
		d.SupplyNamePlural = "Psi";
		d.Type = RaceType::Protoss;
		d.ConsoleSprite.Path = "game_pconsole.t3x";
		d.SupplyIconId = 6;
		d.GasIconId = 3;
		d.AdvisorErrorSounds.SoundPath = "sound/protoss/advisor/paderr";
		d.AdvisorErrorSounds.SoundIds = { 0,1,2,6 };
		d.AdvisorUpdateSounds.SoundPath = "sound/protoss/advisor/padupd";
		d.AdvisorUpdateSounds.SoundIds = { 0,1,2,4 };
		d.GameMusic.push_back({ "music/protoss1.wav" });
		d.GameMusic.push_back({ "music/protoss2.wav" });
		d.GameMusic.push_back({ "music/protoss3.wav" });
		d.CommandIconsLoad = []() { Protoss.CommandIconsAtlas = SpriteDatabase::Load_unit_cmdbtns_pcmdbtns(); };
	}
}

const RaceDef* RaceDatabase::GetRaceDef(RaceType type)
{
	switch (type)
	{
	case RaceType::Terran: return &Terran;
		break;
	case RaceType::Zerg: return &Zerg;
		break;
	case RaceType::Protoss: return &Protoss;
		break;
	default:
		return nullptr;
	}
}
