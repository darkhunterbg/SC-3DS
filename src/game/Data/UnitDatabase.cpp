#include "UnitDatabase.h"
#include "Platform.h"
#include "Generated.h"

UnitDef UnitDatabase::Marine;
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

	u.DeathAnimationDef.GenerateAnimation(a, &u.DeathAnimation);
	u.MovementAnimationDef.GenerateAnimations(a, as, u.MovementAnimations, u.MovementAnimationsShadow, u.MovementAnimationsTeamColor);
}

void UnitDatabase::Init()
{
	Units.push_back(&Marine);
	MarineData();
}

void UnitDatabase::LoadAllUnitResources()
{
	MarineResources();
}
