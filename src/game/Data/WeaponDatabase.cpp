#include "WeaponDatabase.h"
#include "Generated.h"

WeaponDef WeaponDatabase::GaussRifle;
WeaponDef WeaponDatabase::FusionCutter;
std::vector<WeaponDef*> WeaponDatabase::Weapons = { &GaussRifle, &FusionCutter };

static void GaussRifleData() {
	auto& d = WeaponDatabase::GaussRifle;
	d.Name = "Gauss Rifle";
	d.Cooldown = 15;
	d.Sound = { "sound/bullet/tmafir00", 1 , true };

	d.TargetEffectDef.FrameCount = 15;
	d.TargetEffectDef.Directions = 8;
	d.TargetEffectDef.FrameStart = 0;
	d.TargetEffectDef.FrameTime = 1;

	d.LoadGraphicsAction = []() {
		auto sa = SpriteDatabase::Load_unit_bullet_tspark();
		WeaponDatabase::GaussRifle.TargetEffectDef.GenerateAnimations(sa,
			WeaponDatabase::GaussRifle.TargetEffect);
	};
}

static void FusionCutterData() {
	auto& d = WeaponDatabase::FusionCutter;
	d.Name = "Fusion Cutter";
	d.Cooldown = 15;
	d.Sound = { "sound/terran/scv/edrrep", 5  };

	d.TargetEffectDef.FrameCount = 15;
	d.TargetEffectDef.Directions = 8;
	d.TargetEffectDef.FrameStart = 0;
	d.TargetEffectDef.FrameTime = 1;

	d.LoadGraphicsAction = []() {
		auto sa = SpriteDatabase::Load_unit_bullet_tspark();
		WeaponDatabase::FusionCutter.TargetEffectDef.GenerateAnimations(sa,
			WeaponDatabase::FusionCutter.TargetEffect);
	};
}

void WeaponDatabase::Init()
{
	GaussRifleData();
	FusionCutterData();

	for (auto d : Weapons) {
		d->LoadSound();
		d->LoadGraphicsAction();
	}

}
