#include "UnitDatabase.h"
#include "Platform.h"
#include "Generated.h"

UnitDef UnitDatabase::Marine;
UnitDef UnitDatabase::SCV;
std::vector<const UnitDef*> UnitDatabase::Units;

static void MarineData() {
	UnitDef& u = UnitDatabase::Marine;
	u.Name = "Marrine";
	u.Health = 40;
	u.MovementSpeed = 4;
	u.RotationSpeed = 1;
	u.Collider.position = { -9,-10 };
	u.Collider.size = { 17,20 };

	UnitDirectionalAnimationDef& a = u.MovementAnimationDef;
	a.FrameStart = 68;
	a.FrameDuration = 9;
	a.Looping = true;
	a.UnitColorFrameStart = 229 + a.FrameStart;

	u.DeathAnimationDef.FrameStart = 221;
	u.DeathAnimationDef.FrameDuration = 8; 
	u.DeathAnimationDef.UnitColorFrameStart = -1;

	u.DeathSoundDef = { "sound/terran/marine/tmadth", 2 };
	u.SelectedSoundDef = { "sound/terran/marine/tmawht", 4 };
	u.ActionConfirmSoundDef = { "sound/terran/marine/tmayes", 4 };
}
static void MarineResources() {
	auto a = SpriteDatabase::Load_unit_terran_marine();
	auto as = SpriteDatabase::Load_unit_terran_tmashad();

	UnitDef& u = UnitDatabase::Marine;

	u.SelectedSoundDef.LoadSoundClips();
	u.ActionConfirmSoundDef.LoadSoundClips();
	u.DeathSoundDef.LoadSoundClips();

	u.RenderSize = Vector2Int16(a->FrameSize);
	u.DeathAnimationDef.GenerateAnimation(a, nullptr, u.DeathAnimation);
	u.MovementAnimationDef.GenerateAnimations(a, as, u.MovementAnimations);
}


static void SCVData() {
	UnitDef& u = UnitDatabase::SCV;
	u.Name = "SCV";
	u.Health = 60;
	u.MovementSpeed = 5;
	u.RotationSpeed = 1;
	u.Collider.position = { -10,-14 };
	u.Collider.size = { 23,23 };

	UnitDirectionalAnimationDef& a = u.MovementAnimationDef;
	a.FrameStart = 0;
	a.FrameDuration = 1;
	a.Looping = false;
	a.UnitColorFrameStart = 51;
	a.ShadowOffset = { -1, 5 };

	u.DeathSoundDef = { "sound/terran/scv/tscdth", 1 };
	u.SelectedSoundDef = { "sound/terran/scv/tscwht", 4 };
	u.ActionConfirmSoundDef = { "sound/terran/scv/tscyes", 4 };
}
static void SCVResources() {
	auto a = SpriteDatabase::Load_unit_terran_scv();

	UnitDef& u = UnitDatabase::SCV;

	u.SelectedSoundDef.LoadSoundClips();
	u.ActionConfirmSoundDef.LoadSoundClips();
	u.DeathSoundDef.LoadSoundClips();

	u.RenderSize = Vector2Int16(a->FrameSize);
	//u.DeathAnimationDef.GenerateAnimation(a, &u.DeathAnimation);
	u.MovementAnimationDef.GenerateAnimations(a, a, u.MovementAnimations);
}

void UnitDatabase::Init()
{
	Units.push_back(&Marine);
	Units.push_back(&SCV);
	MarineData();
	SCVData();
}

void UnitDatabase::LoadAllUnitResources()
{
	MarineResources();
	SCVResources();
}
