#include "RaceDatabase.h"

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
		d.SupplyBuildingNamePlural = "Supply Depots";
		d.Type = RaceType::Terran;
		d.consolePath= "game\\tconsole";
		d.commandIconsPath = "unit\\cmdbtns\\tcmdbtns";
		d.supplyIconPath = "game\\icons\\tsupply";
		d.gasIconPath = "game\\icons\\tgas";
		d.AdvisorErrorSounds.SoundPath = "sound/terran/advisor/taderr";
		d.AdvisorErrorSounds.SoundIds = { 0,1,2,3,4,6 };
		d.AdvisorUpdateSounds.SoundPath = "sound/terran/advisor/tadupd";
		d.AdvisorUpdateSounds.SoundIds = { 0,1,2,3,4,5,6,7 };
		d.GameMusic.push_back({ "music/terran1.wav" });
		d.GameMusic.push_back({ "music/terran2.wav" });
		d.GameMusic.push_back({ "music/terran3.wav" });

	}
	// ============= Zerg =========================
	{
		RaceDef& d = Zerg;
		d.Name = "Zerg";
		d.SupplyNamePlural = "Controls";
		d.SupplyBuildingNamePlural = "Overlords";
		d.Type = RaceType::Zerg;
		d.consolePath = "game\\zconsole";
		d.commandIconsPath = "unit\\cmdbtns\\zcmdbtns";
		d.supplyIconPath = "game\\icons\\zsupply";
		d.gasIconPath = "game\\icons\\zgas";
		d.AdvisorErrorSounds.SoundPath = "sound/zerg/advisor/zaderr";
		d.AdvisorErrorSounds.SoundIds = { 0,1,2,6 };
		d.AdvisorUpdateSounds.SoundPath = "sound/zerg/advisor/zadupd";
		d.AdvisorUpdateSounds.SoundIds = { 0,1,2,4 };
		d.GameMusic.push_back({ "music/zerg1.wav" });
		d.GameMusic.push_back({ "music/zerg2.wav" });
		d.GameMusic.push_back({ "music/zerg3.wav" });

	}
	// ============= Protoss ======================
	{
		RaceDef& d = Protoss;
		d.Name = "Protoss";
		d.SupplyNamePlural = "Psi";
		d.SupplyBuildingNamePlural = "Pylons";
		d.Type = RaceType::Protoss;
		d.consolePath = "game\\pconsole";
		d.commandIconsPath = "unit\\cmdbtns\\pcmdbtns";
		d.supplyIconPath = "game\\icons\\psupply";
		d.gasIconPath = "game\\icons\\pgas";
		d.AdvisorErrorSounds.SoundPath = "sound/protoss/advisor/paderr";
		d.AdvisorErrorSounds.SoundIds = { 0,1,2,6 };
		d.AdvisorUpdateSounds.SoundPath = "sound/protoss/advisor/padupd";
		d.AdvisorUpdateSounds.SoundIds = { 0,1,2,4 };
		d.GameMusic.push_back({ "music/protoss1.wav" });
		d.GameMusic.push_back({ "music/protoss2.wav" });
		d.GameMusic.push_back({ "music/protoss3.wav" });
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
