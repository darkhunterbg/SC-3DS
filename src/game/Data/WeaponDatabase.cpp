#include "WeaponDatabase.h"
#include "Generated.h"

WeaponDef WeaponDatabase::GaussRifle;
WeaponDef WeaponDatabase::FusionCutter;
std::vector<WeaponDef*> WeaponDatabase::Weapons = { &GaussRifle, &FusionCutter };

static void GaussRifleData() {
	auto& d = WeaponDatabase::GaussRifle;
	d.Name = "Gauss Rifle";
	d.Cooldown = 15;
	d.MaxRange = 4;
	d.Damage = 6;
	d.Sound = { "sound/bullet/tmafir00", 1 , true };

	d.TargetEffectDef.FrameCount = 15;
	d.TargetEffectDef.Directions = 8;
	d.TargetEffectDef.FrameStart = 0;
	d.TargetEffectDef.FrameTime = 1;

	d.IconId = 288;

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
	d.MaxRange = 0;
	d.Damage = 5;
	d.Sound = { "sound/terran/scv/edrrep", 5 };

	d.TargetEffectDef.FrameCount = 10;
	d.TargetEffectDef.Directions = 1;
	d.TargetEffectDef.FrameStart = 0;
	d.TargetEffectDef.FrameTime = 1;

	d.IconId = 329;

	d.LoadGraphicsAction = []() {
		auto sa = SpriteDatabase::Load_unit_bullet_scvspark();
		WeaponDatabase::FusionCutter.TargetEffectDef.GenerateAnimations(sa,
			WeaponDatabase::FusionCutter.TargetEffect);	};
}

void WeaponDatabase::Init()
{
	GaussRifleData();
	FusionCutterData();
	auto s = SpriteDatabase::Load_unit_cmdbtns_cmdicons();

	for (auto d : Weapons) {
		d->LoadSound();
		d->LoadGraphicsAction();

		d->Icon = s->GetFrame(d->IconId).sprite;
	}

}
