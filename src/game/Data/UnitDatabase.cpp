#include "UnitDatabase.h"
#include "Platform.h"

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
	auto a = Platform::LoadAtlas("marine.t3x");

	UnitDef& u = UnitDatabase::Marine;

	u.DeathAnimation.AddSprite(a->GetSprite(221), { 22,19 });
	u.DeathAnimation.AddSprite(a->GetSprite(222), { 23,19 });
	u.DeathAnimation.AddSprite(a->GetSprite(223), { 17,17 });
	u.DeathAnimation.AddSprite(a->GetSprite(224), { 12,14 });
	u.DeathAnimation.AddSprite(a->GetSprite(225), { 7,15 });
	u.DeathAnimation.AddSprite(a->GetSprite(226), { 2,17 });
	u.DeathAnimation.AddSprite(a->GetSprite(227), { 0,19 });
	u.DeathAnimation.AddSprite(a->GetSprite(228), { 0,20 });
	u.DeathAnimation.frameSize = { 64,64 };
	for (auto& c : u.DeathAnimation.GetFrames())
	{
		c.offset -= (u.DeathAnimation.frameSize) / 2;
	}

	for (int i = 0; i <= 16; ++i) {
		u.MovementAnimations[i].frameSize = { 64,64 };
		u.MovementAnimations[i].looping = true;
		u.MovementAnimations[i].frameDuration = 1;
		for (int j = 0; j < 9; ++j)
			u.MovementAnimations[i].AddSprite(a->GetSprite(i + j * 17 + 68));
	}

	for (int i = 17; i < 32; ++i) {
		u.MovementAnimations[i].frameSize = { 64,64 };
		u.MovementAnimations[i].looping = true;
		u.MovementAnimations[i].frameDuration = 1;
		for (int j = 0; j < 9; ++j)
			u.MovementAnimations[i].AddSprite(a->GetSprite(32 - i + j * 17 + 68), { 0,0 }, true);
	}

	for (int i = 0; i < 32; ++i) {
		for (int j = 0; j < u.MovementAnimations[i].GetFrameCount(); ++j)
		{
			auto& c = u.MovementAnimations[i];
			auto& f = u.MovementAnimations[i].GetFrame(j);
			c.SetFrameOffset(j, (c.frameSize - f.sprite.rect.size) / 2 - c.frameSize / 2);
		}

	}
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
