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

}
static void MarineResources() {
	auto a = SpriteDatabase::Load_unit_terran_marine();
	auto as = SpriteDatabase::Load_unit_terran_tmashad();

	UnitDef& u = UnitDatabase::Marine;

	for (int i = 0; i < 8; ++i)
		u.DeathAnimation.AddFrameCentered(a->GetFrame(221 + i), a->FrameSize);

	//for (auto& c : u.DeathAnimation.GetFrames())
	//	c.offset -= (a->FrameSize) / 2;

	for (int i = 0; i <= 16; ++i) {
		u.MovementAnimations[i].looping = true;

		for (int j = 0; j < 9; ++j) {
			u.MovementAnimations[i].AddFrameCentered(a->GetFrame(i + j * 17 + 68), a->FrameSize);
			u.MovementAnimationsTeamColor[i].AddFrameCentered(a->GetFrame(i + j * 17 + 68 + 229), a->FrameSize);
		}
	}

	for (int i = 17; i < 32; ++i) {
		u.MovementAnimations[i].looping = true;
		u.MovementAnimations[i].frameDuration = 1;
		for (int j = 0; j < 9; ++j) {
			u.MovementAnimations[i].AddFrameCentered(a->GetFrame(32 - i + j * 17 + 68), a->FrameSize, true);
			u.MovementAnimationsTeamColor[i].AddFrameCentered(a->GetFrame(32 - i + j * 17 + 68 + 229), a->FrameSize);
		}

	}



	for (int i = 0; i < 16; ++i) {
		u.MovementAnimationsShadow[i].looping = true;
		u.MovementAnimationsShadow[i].frameDuration = 1;
		for (int j = 0; j < 9; ++j)
			u.MovementAnimationsShadow[i].AddFrameCentered(as->GetFrame(i + j * 17 + 68), as->FrameSize);

	}

	for (int i = 17; i < 32; ++i) {
		u.MovementAnimationsShadow[i].looping = true;
		u.MovementAnimationsShadow[i].frameDuration = 1;
		for (int j = 0; j < 9; ++j)
			u.MovementAnimationsShadow[i].AddFrameCentered(as->GetFrame(32 - i + j * 17 + 68), as->FrameSize, true);

	}

	//for (int i = 0; i < 32; ++i) {
	//	for (int j = 0; j < u.MovementAnimations[i].GetFrameCount(); ++j)
	//	{
	//		auto& c = u.MovementAnimations[i];
	//		auto& f = u.MovementAnimations[i].GetFrame(j);
	//		//c.SetFrameOffset(j, f.offset - (a->FrameSize) / 2);
	//	}

	//}
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
