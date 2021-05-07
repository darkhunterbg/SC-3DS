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

	DirectionalAnimationDef& a = u.MovementAnimationDef;
	a.FrameStart = 68;
	a.FrameDuration = 9;
	a.Looping = true;
	a.UnitColorFrameStart = 229 + a.FrameStart;

	u.DeathAnimationDef = { 221 ,8, false };
}
static void MarineResources() {
	auto a = SpriteDatabase::Load_unit_terran_marine();
	auto as = SpriteDatabase::Load_unit_terran_tmashad();

	UnitDef& u = UnitDatabase::Marine;

	u.RenderSize = a->FrameSize;
	u.DeathAnimationDef.GenerateAnimation(a, &u.DeathAnimation);
	u.MovementAnimationDef.GenerateAnimations(a, as, u.MovementAnimations, u.MovementAnimationsShadow, u.MovementAnimationsTeamColor);
}


static void SCVData() {
	UnitDef& u = UnitDatabase::SCV;
	u.Name = "SCV";
	u.Health = 60;
	u.MovementSpeed = 5;
	u.RotationSpeed = 1;
	u.Collider.position = { -10,-14 };
	u.Collider.size = { 23,23 };

	DirectionalAnimationDef& a = u.MovementAnimationDef;
	a.FrameStart = 0;
	a.FrameDuration = 1;
	a.Looping = false;
	a.UnitColorFrameStart = 51;
	a.ShadowAdditionalOffset = { -1, 5 };

	u.DeathAnimationDef = { 0 ,0, false };
}
static void SCVResources() {
	auto a = SpriteDatabase::Load_unit_terran_scv();

	UnitDef& u = UnitDatabase::SCV;

	u.RenderSize = a->FrameSize;
	//u.DeathAnimationDef.GenerateAnimation(a, &u.DeathAnimation);
	u.MovementAnimationDef.GenerateAnimations(a, a, u.MovementAnimations, u.MovementAnimationsShadow, u.MovementAnimationsTeamColor);
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
