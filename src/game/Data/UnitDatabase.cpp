#include "UnitDatabase.h"
#include "Platform.h"
#include "GraphicsDatabase.h"
#include "WeaponDatabase.h"

UnitDef UnitDatabase::Marine;
UnitDef UnitDatabase::SCV;
UnitDef UnitDatabase::CommandCenter;
UnitDef UnitDatabase::MineralField1;

std::vector<UnitDef*> UnitDatabase::Units = {
	&Marine, &SCV, &CommandCenter, &MineralField1
};

static void MarineData() {
	UnitDef& u = UnitDatabase::Marine;
	u.Name = "Terran Marine";
	u.Health = 40;
	u.MovementSpeed = 4;
	u.RotationSpeed = 1;
	u.SetUseSupply(1);
	u.Vision = 7;
	u.Weapon = &WeaponDatabase::GaussRifle;

	u.Sounds.Death = { "sound/terran/marine/tmadth", 2 };
	u.Sounds.What = { "sound/terran/marine/tmawht", 4 };
	u.Sounds.Yes = { "sound/terran/marine/tmayes", 4 };
	u.Graphics = &GraphicsDatabase::Marine;

	u.AudioPriority = 1;
}
static void SCVData() {
	UnitDef& u = UnitDatabase::SCV;
	u.Name = "Terran SCV";
	u.Health = 60;
	u.MovementSpeed = 5;
	u.RotationSpeed = 1;
	u.SetUseSupply(1);
	u.Vision = 7;
	u.Weapon = &WeaponDatabase::FusionCutter;

	u.Sounds.Death = { "sound/terran/scv/tscdth", 1 };
	u.Sounds.What = { "sound/terran/scv/tscwht", 4 };
	u.Sounds.Yes = { "sound/terran/scv/tscyes", 4 };
	u.Graphics = &GraphicsDatabase::SCV;
}
static void CommandCenterData() {
	UnitDef& u = UnitDatabase::CommandCenter;
	u.Name = "Terran Command Center";
	u.Health = 1500;
	u.MovementSpeed = 0;
	u.RotationSpeed = 0;
	u.Vision = 10;
	u.SetPovideSupply(10);
	u.IsBuilding = true;

	u.Sounds.Death = { "sound/misc/explo4", 1 , true };
	u.Sounds.What = { "sound/misc/button", 1 , true };
	u.Graphics = &GraphicsDatabase::CommandCenter;
}
static void MineralField1Data() {
	UnitDef& u = UnitDatabase::MineralField1;
	u.Name = "Mineral Field";
	u.Health = 100000;
	u.MovementSpeed = 0;
	u.RotationSpeed = 0;
	u.Vision = 0;

	u.IsBuilding = true;
	u.IsResourceContainer = true;

	u.Sounds.What = { "sound/misc/button", 1 , true };
	u.Graphics = &GraphicsDatabase::Minerals1;
}

void UnitDatabase::Init()
{
	MarineData();
	SCVData();
	CommandCenterData();
	MineralField1Data();
}

void UnitDatabase::LoadUnitResources(UnitDef& def) {
	def.Graphics->LoadResourcesAction();
	def.LoadAllSounds();
}

void UnitDatabase::LoadAllUnitResources()
{
	for (auto unit : Units) {

		LoadUnitResources(*unit);
	}
}
