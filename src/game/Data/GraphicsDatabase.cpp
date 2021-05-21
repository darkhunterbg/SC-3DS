#include "GraphicsDatabase.h"
#include "Generated.h"

UnitGraphicsDef GraphicsDatabase::Marine;
UnitGraphicsDef GraphicsDatabase::SCV;
UnitGraphicsDef GraphicsDatabase::CommandCenter;
std::vector<UnitGraphicsDef*> GraphicsDatabase::Units =
{
	&Marine, &SCV, &CommandCenter
};

// ======================= Terran Marine ==========================================

static void MarineResources() {
	auto a = SpriteDatabase::Load_unit_terran_marine();
	auto as = SpriteDatabase::Load_unit_terran_tmashad();
	UnitGraphicsDef& d = GraphicsDatabase::Marine;
	d.RenderSize = Vector2Int16(a->FrameSize);
	d.DeathAnimationDef.GenerateAnimation(a, nullptr, d.DeathAnimation);
	d.MovementAnimationDef.GenerateAnimations(a, as, d.MovementAnimations);
	d.AttackAnimationDef.GenerateAnimations(a, as, d.AttackAnimations);
	d.AttackAnimationDef.FrameStart = 34;
	d.AttackAnimationDef.UnitColorFrameStart = 229 + d.AttackAnimationDef.FrameStart;
	d.AttackAnimationDef.GenerateAnimations(a, as, d.AttackAnimations);
	d.IdleAnimationDef.GenerateAnimations(a, as, d.IdleAnimations);
	d.DeathAfterEffect.Def.GenerateAnimation(a, d.DeathAfterEffect.Clip);
}
static void MarineData() {
	UnitGraphicsDef& d = GraphicsDatabase::Marine;
	d.Collider.position = { -9,-10 };
	d.Collider.size = { 17,20 };

	d.MovementAnimationDef.FrameStart = 68;
	d.MovementAnimationDef.FrameCount = 9;
	d.MovementAnimationDef.Looping = true;
	d.MovementAnimationDef.UnitColorFrameStart = 229 + d.MovementAnimationDef.FrameStart;

	d.AttackAnimationDef.FrameStart = 51;
	d.AttackAnimationDef.FrameCount = 1;
	d.AttackAnimationDef.UnitColorFrameStart = 229 + d.AttackAnimationDef.FrameStart;
	d.AttackAnimationDef.FrameTime = 2;
	d.AttackAnimationDef.Looping = true;

	d.IdleAnimationDef.FrameStart = 0;
	d.IdleAnimationDef.FrameCount = 1;
	d.IdleAnimationDef.FrameTime = 1;
	d.IdleAnimationDef.UnitColorFrameStart = 229 + d.IdleAnimationDef.FrameStart;
	//d.IdleAnimationDef.Looping = true;

	d.DeathAnimationDef.FrameStart = 221;
	d.DeathAnimationDef.FrameCount = 8;
	d.DeathAnimationDef.UnitColorFrameStart = -1;
	d.DeathAnimationDef.FrameTime = 2;

	d.DeathAfterEffect.Def.FrameStart = 452;
	d.DeathAfterEffect.Def.FrameCount = 3;
	d.DeathAfterEffect.Def.FrameTime = 48;
	d.DeathAfterEffect.Depth = -1;

	d.LoadResourcesAction = MarineResources;
}

// ============================ Terran SCV ================================

static void SCVResources() {
	auto a = SpriteDatabase::Load_unit_terran_scv();
	auto ad = SpriteDatabase::Load_unit_thingy_tbangs();

	UnitGraphicsDef& d = GraphicsDatabase::SCV;

	d.RenderSize = Vector2Int16(a->FrameSize);
	d.DeathAfterEffect.Def.GenerateAnimation(ad,  d.DeathAfterEffect.Clip);
	d.MovementAnimationDef.GenerateAnimations(a, a, d.MovementAnimations);
	d.MovementGlowAnimationDef.GenerateAnimations(a, d.MovementGlowAnimations);
	d.IdleAnimationDef.GenerateAnimations(a,a, d.IdleAnimations);
}
static void SCVData() {
	UnitGraphicsDef& d = GraphicsDatabase::SCV;
	d.Collider.position = { -10,-14 };
	d.Collider.size = { 23,23 };

	d.MovementAnimationDef.FrameStart = 0;
	d.MovementAnimationDef.FrameCount = 1;
	d.MovementAnimationDef.UnitColorFrameStart = 51;
	d.MovementAnimationDef.ShadowOffset = { -1, 5 };

	d.IdleAnimationDef.FrameStart = 0;
	d.IdleAnimationDef.FrameCount = 1;
	d.IdleAnimationDef.UnitColorFrameStart = 51;
	d.IdleAnimationDef.ShadowOffset = { -1, 5 };

	d.MovementGlowAnimationDef.FrameStart = 102;
	d.MovementGlowAnimationDef.FrameCount = 4;
	d.MovementGlowAnimationDef.Looping = true;

	d.DeathAfterEffect.Def.FrameStart = 0;
	d.DeathAfterEffect.Def.FrameCount = 9;
	d.DeathAnimationDef.FrameTime = 2;

	d.LoadResourcesAction = SCVResources;
}

// ============================ Terran Command Center ==========================
static void CommandCenterResources() {
	auto a = SpriteDatabase::Load_unit_terran_control();
	auto as = SpriteDatabase::Load_unit_terran_tccshad();
	auto ad = SpriteDatabase::Load_unit_thingy_tbangx();

	UnitGraphicsDef& d = GraphicsDatabase::CommandCenter;

	d.RenderSize = Vector2Int16(a->FrameSize);
	d.IdleAnimationDef.GenerateAnimations(a, as, d.IdleAnimations);
	d.DeathAfterEffect.Def.GenerateAnimation(ad,  d.DeathAfterEffect.Clip);
}
static void CommandCenterData() {
	UnitGraphicsDef& d = GraphicsDatabase::CommandCenter;
	d.Collider.position = { -58,-41 };
	d.Collider.size = { 116,82 };

	d.IdleAnimationDef.FrameStart = 0;
	d.IdleAnimationDef.FrameCount = 1;
	d.IdleAnimationDef.UnitColorFrameStart = 6;
	d.IdleAnimationDef.MultiDirectional = false;

	d.DeathAfterEffect.Def.FrameStart = 0;
	d.DeathAfterEffect.Def.FrameCount = 13;
	d.DeathAfterEffect.Def.FrameTime = 2;

	d.LoadResourcesAction = CommandCenterResources;
}
// ==============================================================================

void GraphicsDatabase::Init()
{
	MarineData();
	SCVData();
	CommandCenterData();

	for (auto unit : Units) {
		Vector2Int16 size = unit->Collider.size;
		if (size.x % 32 != 0)
			size.x += 32;

		if (size.y % 32 != 0)
			size.y += 32;

		unit->MinimapBB.size = Vector2Int16(size >> 5);
		unit->MinimapBB.size = unit->MinimapBB.size.Max(2, 2);
		unit->MinimapBB.position -= unit->MinimapBB.size / 2;
	}
}

void GraphicsDatabase::LoadAllGraphicsResources()
{
	for (auto& unit : Units) {
		unit->LoadResourcesAction();
	}
}
