#include "WeaponDatabase.h"

WeaponDef WeaponDatabase::GaussRifle;
WeaponDef WeaponDatabase::FusionCutter;
std::vector<WeaponDef*> WeaponDatabase::Weapons = { &GaussRifle, &FusionCutter };

static void GaussRifleData() {
	auto& d = WeaponDatabase::GaussRifle;
	d.Name = "Gauss Rifle";
	d.Cooldown = 15;
	d.Sound = { "sound/bullet/tmafir00", 1 , true };
}

static void FusionCutterData() {
	auto& d = WeaponDatabase::FusionCutter;
	d.Name = "Fusion Cutter";
	d.Cooldown = 15;
	d.Sound = { "sound/terran/scv/edrrep", 5  };
}

void WeaponDatabase::Init()
{
	GaussRifleData();
	FusionCutterData();

	for (auto d : Weapons)
		d->LoadSound();
}
