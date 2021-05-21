#include "UnitDatabase.h"
#include "Platform.h"
#include "GraphicsDatabase.h"

UnitDef UnitDatabase::Marine;
UnitDef UnitDatabase::SCV;
UnitDef UnitDatabase::CommandCenter;
std::vector<UnitDef*> UnitDatabase::Units = {
	&Marine, &SCV, &CommandCenter
};

static void MarineData() {
	UnitDef& u = UnitDatabase::Marine;
	u.Name = "Terran Marine";
	u.Health = 40;
	u.MovementSpeed = 4;
	u.RotationSpeed = 1;
	u.SetUseSupply(1);
	u.Vision = 7;

	u.Sounds.Death = { "sound/terran/marine/tmadth", 2 };
	u.Sounds.What = { "sound/terran/marine/tmawht", 4 };
	u.Sounds.Yes = { "sound/terran/marine/tmayes", 4 };
	u.Graphics = &GraphicsDatabase::Marine;
}
static void SCVData() {
	UnitDef& u = UnitDatabase::SCV;
	u.Name = "Terran SCV";
	u.Health = 60;
	u.MovementSpeed = 5;
	u.RotationSpeed = 1;
	u.SetUseSupply(1);
	u.Vision = 7;

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

	u.Sounds.Death = { "sound/misc/explo4", 1 , true };
	u.Sounds.What = { "sound/misc/button", 1 , true };
	u.Graphics = &GraphicsDatabase::CommandCenter;
}

void UnitDatabase::Init()
{
	MarineData();
	SCVData();
	CommandCenterData();
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
