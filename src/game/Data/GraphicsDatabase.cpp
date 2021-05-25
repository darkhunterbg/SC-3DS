#include "GraphicsDatabase.h"
#include "Generated.h"

UnitGraphicsDef GraphicsDatabase::Marine;
UnitGraphicsDef GraphicsDatabase::SCV;
UnitGraphicsDef GraphicsDatabase::CommandCenter;
UnitGraphicsDef GraphicsDatabase::Minerals1;
CursorGraphics GraphicsDatabase::Cursor;

std::vector<UnitGraphicsDef*> GraphicsDatabase::Units =
{
	&Marine, &SCV, &CommandCenter, & Minerals1
};

// ============================= Cursor ===========================================

static int cursorAtlasCounter = 0;

static void InitCursorAnimation(const SpriteFrameAtlas* atlas, AnimationClip& clip, int frames) {

	for (int i = cursorAtlasCounter; i < cursorAtlasCounter + frames; ++i) {
		clip.AddFrameCentered(atlas->GetFrame(i), Vector2Int16(atlas->FrameSize));
	}
	cursorAtlasCounter += frames;
	clip.looping = true;
}

void CursorGraphics::Load() {
	const auto atlas = SpriteDatabase::Load_cursor();

	cursorAtlasCounter = 0;

	InitCursorAnimation(atlas, arrow, 5);
	InitCursorAnimation(atlas, drag, 4);
	InitCursorAnimation(atlas, illegal, 5);
	InitCursorAnimation(atlas, magg, 14);
	InitCursorAnimation(atlas, magr, 14);
	InitCursorAnimation(atlas, magy, 14);


	InitCursorAnimation(atlas, scrolld, 2);
	InitCursorAnimation(atlas, scrolldl, 2);
	InitCursorAnimation(atlas, scrolldr, 2);
	InitCursorAnimation(atlas, scrolll, 2);
	InitCursorAnimation(atlas, scrollr, 2);
	InitCursorAnimation(atlas, scrollu, 2);
	InitCursorAnimation(atlas, scrollul, 2);
	InitCursorAnimation(atlas, scrollur, 2);
	InitCursorAnimation(atlas, targg, 2);
	InitCursorAnimation(atlas, targn, 1);
	InitCursorAnimation(atlas, targr, 2);
	InitCursorAnimation(atlas, targy, 2);

	scrollAnim[0] = &scrollul;
	scrollAnim[1] = &scrollu;
	scrollAnim[2] = &scrollur;
	scrollAnim[3] = &scrolll;
	scrollAnim[4] = nullptr;;
	scrollAnim[5] = &scrollr;
	scrollAnim[6] = &scrolldl;
	scrollAnim[7] = &scrolld;
	scrollAnim[8] = &scrolldr;
}

// ======================= Terran Marine ==========================================

static void MarineResources() {
	auto a = SpriteDatabase::Load_unit_terran_marine();
	auto as = SpriteDatabase::Load_unit_terran_tmashad();
	UnitGraphicsDef& d = GraphicsDatabase::Marine;
	d.RenderSize = Vector2Int16(a->FrameSize);
	d.DeathAnimationDef.GenerateAnimation(a, nullptr, d.DeathAnimation);
	d.MovementAnimationDef.GenerateAnimations(a, as, d.MovementAnimations);

	d.Selection.Atlas = SpriteDatabase::Load_unit_thingy_o022();

	for (int i = 0; i < 2; ++i) {
		d.AttackAnimationDef.GenerateAnimations(a, as, d.AttackAnimations);
		d.AttackAnimationDef.FrameStart = 34;
		d.AttackAnimationDef.UnitColorFrameStart = 229 + d.AttackAnimationDef.FrameStart;
		d.AttackAnimationDef.GenerateAnimations(a, as, d.AttackAnimations);
		d.AttackAnimationDef.FrameStart = 51;
		d.AttackAnimationDef.UnitColorFrameStart = 229 + d.AttackAnimationDef.FrameStart;
	}
	d.IdleAnimationDef.GenerateAnimations(a, as, d.IdleAnimations);
	d.DeathAfterEffect.Def.GenerateAnimation(a, d.DeathAfterEffect.Clip);
}
static void MarineData() {
	UnitGraphicsDef& d = GraphicsDatabase::Marine;
	d.Collider.position = { -9,-10 };
	d.Collider.size = { 17,20 };

	d.MovementAnimationDef.FrameStart = 68;
	d.MovementAnimationDef.ShadowFrameStart = 68;
	d.MovementAnimationDef.FrameCount = 9;
	d.MovementAnimationDef.Looping = true;
	d.MovementAnimationDef.UnitColorFrameStart = 229 + d.MovementAnimationDef.FrameStart;

	d.AttackAnimationDef.FrameStart = 51;
	d.AttackAnimationDef.ShadowFrameStart = 51;
	d.AttackAnimationDef.FrameCount = 1;
	d.AttackAnimationDef.UnitColorFrameStart = 229 + d.AttackAnimationDef.FrameStart;
	d.AttackAnimationDef.FrameTime = 2;
	//d.AttackAnimationDef.Looping = true;

	d.IdleAnimationDef.FrameStart = 0;
	d.IdleAnimationDef.ShadowFrameStart = 0;
	d.IdleAnimationDef.FrameCount = 1;
	d.IdleAnimationDef.FrameTime = 1;
	d.IdleAnimationDef.UnitColorFrameStart = 229 + d.IdleAnimationDef.FrameStart;
	//d.IdleAnimationDef.Looping = true;

	d.DeathAnimationDef.FrameStart = 221;
	d.DeathAnimationDef.ShadowFrameStart = 221;
	d.DeathAnimationDef.FrameCount = 8;
	d.DeathAnimationDef.UnitColorFrameStart = -1;
	d.DeathAnimationDef.FrameTime = 2;

	d.DeathAfterEffect.Def.FrameStart = 452;
	d.DeathAfterEffect.Def.FrameCount = 3;
	d.DeathAfterEffect.Def.FrameTime = 48;
	d.DeathAfterEffect.Depth = -1;

	d.Selection.VecticalOffset = 9;

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

	d.Selection.Atlas = SpriteDatabase::Load_unit_thingy_o022();

	d.AttackAnimationDef.GenerateAnimations(a, a, d.AttackAnimations);
	d.AttackAnimationDef.FrameCount = 1;
	d.AttackAnimationDef.GenerateAnimations(a, a, d.AttackAnimations);
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

	d.AttackAnimationDef.FrameStart = 17;
	d.AttackAnimationDef.FrameCount = 2;
	d.AttackAnimationDef.UnitColorFrameStart = 51 + d.AttackAnimationDef.FrameStart;
	d.AttackAnimationDef.FrameTime = 2;
	d.AttackAnimationDef.ShadowOffset = { -1, 5 };


	d.DeathAfterEffect.Def.FrameStart = 0;
	d.DeathAfterEffect.Def.FrameCount = 9;
	d.DeathAnimationDef.FrameTime = 2;

	d.Selection.VecticalOffset = 11;

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
	d.IdleAnimationDef.ShadowFrameStart = 0;
	d.IdleAnimationDef.FrameCount = 1;
	d.IdleAnimationDef.UnitColorFrameStart = 6;
	d.IdleAnimationDef.MultiDirectional = false;

	d.DeathAfterEffect.Def.FrameStart = 0;
	d.DeathAfterEffect.Def.FrameCount = 13;
	d.DeathAfterEffect.Def.FrameTime = 2;

	d.LoadResourcesAction = CommandCenterResources;
}

// ============================ Mineral Fields ===============================

static void MineralsResources() {
	auto a = SpriteDatabase::Load_unit_neutral_min01();
	auto as = SpriteDatabase::Load_unit_neutral_min01sha();

	UnitGraphicsDef& d = GraphicsDatabase::Minerals1;

	d.RenderSize = Vector2Int16(as->FrameSize);
	d.IdleAnimationDef.GenerateAnimations(a, as, d.IdleAnimations);
}

static void MineralsData() {
	UnitGraphicsDef& d = GraphicsDatabase::Minerals1;
	d.Collider.position = { -32,-16 };
	d.Collider.size = { 63,31 };

	d.IdleAnimationDef.FrameStart = 0;
	d.IdleAnimationDef.ShadowFrameStart = 0;
	d.IdleAnimationDef.FrameCount = 1;
	d.IdleAnimationDef.UnitColorFrameStart = -1;
	d.IdleAnimationDef.MultiDirectional = false;

	d.LoadResourcesAction = MineralsResources;
}

// ==============================================================================

void GraphicsDatabase::Init()
{
	MarineData();
	SCVData();
	CommandCenterData();
	MineralsData();

	Cursor.Load();
}

void GraphicsDatabase::LoadAllGraphicsResources()
{
	for (auto& unit : Units) {
		unit->LoadResourcesAction();
	}
}
