#include "UnitDatabase.h"
#include "Platform.h"
#include "GraphicsDatabase.h"

UnitDef UnitDatabase::Marine;
UnitDef UnitDatabase::SCV;
std::vector<UnitDef*> UnitDatabase::Units = {
	&Marine, &SCV
};

static void MarineData() {
	UnitDef& u = UnitDatabase::Marine;
	u.Name = "Terran Marine";
	u.Health = 40;
	u.MovementSpeed = 4;
	u.RotationSpeed = 1;
	u.SetUseSupply(1);

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

	u.Sounds.Death = { "sound/terran/scv/tscdth", 1 };
	u.Sounds.What = { "sound/terran/scv/tscwht", 4 };
	u.Sounds.Yes = { "sound/terran/scv/tscyes", 4 };
	u.Graphics = &GraphicsDatabase::SCV;

}


void UnitDatabase::Init()
{
	MarineData();
	SCVData();
}

void UnitDatabase::LoadAllUnitResources()
{
	

	for (auto& unit : Units) {
		unit->Graphics->LoadResourcesAction();
		unit->LoadAllSounds();
	}
}
