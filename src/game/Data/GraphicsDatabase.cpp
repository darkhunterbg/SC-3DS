#include "GraphicsDatabase.h"
#include "Generated.h"

UnitGraphicsDef GraphicsDatabase::Marine;
UnitGraphicsDef GraphicsDatabase::SCV;
std::vector<const UnitGraphicsDef*> GraphicsDatabase::Units =
{
	&Marine, &SCV
};

static void MarineResources() {
	auto a = SpriteDatabase::Load_unit_terran_marine();
	auto as = SpriteDatabase::Load_unit_terran_tmashad();
	UnitGraphicsDef& d = GraphicsDatabase::Marine;
	d.RenderSize = Vector2Int16(a->FrameSize);
	d.DeathAnimationDef.GenerateAnimation(a, nullptr, d.DeathAnimation);
	d.MovementAnimationDef.GenerateAnimations(a, as, d.MovementAnimations);
}

static void MarineData() {
	UnitGraphicsDef& d = GraphicsDatabase::Marine;
	d.Collider.position = { -9,-10 };
	d.Collider.size = { 17,20 };

	d.MovementAnimationDef.FrameStart = 68;
	d.MovementAnimationDef.FrameCount = 9;
	d.MovementAnimationDef.Looping = true;
	d.MovementAnimationDef.UnitColorFrameStart = 229 + d.MovementAnimationDef.FrameStart;

	d.DeathAnimationDef.FrameStart = 221;
	d.DeathAnimationDef.FrameCount = 8;
	d.DeathAnimationDef.UnitColorFrameStart = -1;
	d.DeathAnimationDef.FrameTime = 2;

	d.LoadResourcesAction = MarineResources;
}

static void SCVResources() {
	auto a = SpriteDatabase::Load_unit_terran_scv();
	auto ad = SpriteDatabase::Load_unit_thingy_tbangs();

	UnitGraphicsDef& d = GraphicsDatabase::SCV;

	d.RenderSize = Vector2Int16(a->FrameSize);
	d.DeathAnimationDef.GenerateAnimation(ad, nullptr, d.DeathAnimation);
	d.MovementAnimationDef.GenerateAnimations(a, a, d.MovementAnimations);

}
static void SCVData() {
	UnitGraphicsDef& d = GraphicsDatabase::SCV;
	d.Collider.position = { -10,-14 };
	d.Collider.size = { 23,23 };

	d.MovementAnimationDef.FrameStart = 0;
	d.MovementAnimationDef.FrameCount = 1;
	d.MovementAnimationDef.Looping = false;
	d.MovementAnimationDef.UnitColorFrameStart = 51;
	d.MovementAnimationDef.ShadowOffset = { -1, 5 };

	d.DeathAnimationDef.FrameStart = 0;
	d.DeathAnimationDef.FrameCount = 9;
	d.DeathAnimationDef.UnitColorFrameStart = -1;
	d.DeathAnimationDef.FrameTime = 2;

	d.LoadResourcesAction = SCVResources;
}

void GraphicsDatabase::Init()
{
	MarineData();
	SCVData();
}

void GraphicsDatabase::LoadAllGraphicsResources()
{
	for (auto& unit : Units) {
		unit->LoadResourcesAction();
	}
}
