#include "AnimationPlayer.h"

#include <vector>

#include "EntityManager.h"
#include "../Data/AssetDataDefs.h"
#include "../Data/GameDatabase.h"
#include "EntityUtil.h"

typedef void(*InstructionAction)(const InstructionParams& params, EntityId id, EntityManager& em);

static const Vector2 movementTable32[]{
	{0,-1}, {0,-1},
	{0.7,-0.7},{0.7,-0.7},{0.7,-0.7},{0.7,-0.7},
	{1,0}, {1,0}, {1,0}, {1,0},
	{0.7,0.7},{0.7,0.7},{0.7,0.7},{0.7,0.7},
	{0,1},{0,1},{0,1},{0,1},
	{-0.7,0.7},{-0.7,0.7},{-0.7,0.7},{-0.7,0.7},
	{-1,0}, {-1,0}, {-1,0}, {-1,0},
	{-0.7,-0.7},{-0.7,-0.7},{-0.7,-0.7},{-0.7,-0.7},
	{0,-1},{0,-1}

};
struct AnimInstruction {
	InstructionParams params;
	uint8_t id;
};

static std::vector<AnimInstruction> instructionCache;

void AnimationPlayer::BuildInstructionCache(const Span<AnimInstructionDef> instructions)
{
	instructionCache.clear();
	instructionCache.reserve(instructions.Size());

	for (const AnimInstructionDef& def : instructions)
	{
		instructionCache.push_back({ def.param, def.id });
	}
}

// ================================= Instructions =======================================

static void Frame(const InstructionParams& params, EntityId id, EntityManager& em)
{
	auto& anim = em.AnimationSystem.GetComponent(id);
	EntityUtil::SetAnimationFrame(id, params.shorts[0]);
}
static void Wait(const InstructionParams& params, EntityId id, EntityManager& em)
{
	em.AnimationSystem.GetComponent(id).wait = params.shorts[0];
}
static void WaitRandom(const InstructionParams& params, EntityId id, EntityManager& em)
{
	uint16_t i = (uint16_t)em.rand.Next(params.shorts[0], params.shorts[1]);
	em.AnimationSystem.GetComponent(id).wait = i;
}
static void Face(const InstructionParams& params, EntityId id, EntityManager& em)
{
	EntityUtil::SetOrientation(id, (int)params.bytes[0]);
}
static void TurnCW(const InstructionParams& params, EntityId id, EntityManager& em)
{
	int orientation = em.GetOrientation(id);

	EntityUtil::SetOrientation(id, orientation + (int)params.bytes[0]);
}
static void TurnCCW(const InstructionParams& params, EntityId id, EntityManager& em)
{
	int orientation = em.GetOrientation(id);
	EntityUtil::SetOrientation(id, orientation - (int)params.bytes[0]);
}

static void GoTo(const InstructionParams& params, EntityId id, EntityManager& em)
{
	auto& anim = em.AnimationSystem.GetComponent(id);
	anim.instructionCounter = anim.instructionStart + params.bytes[0];
	--anim.instructionCounter;
}
static void GoToRandom(const InstructionParams& params, EntityId id, EntityManager& em)
{
	auto& anim = em.AnimationSystem.GetComponent(id);
	int chance = params.bytes[0];

	int roll = em.rand.Next(0, 100);

	if (roll < chance)
	{
		// TODO: FIX THIS
		anim.instructionCounter = anim.instructionStart + params.bytes[2];
		--anim.instructionCounter;
	}
}

static void Attack(const InstructionParams& params, EntityId id, EntityManager& em)
{
	em.UnitSystem.UnitAttackEvent(id);
}

static void PlaySound(const InstructionParams& params, EntityId id, EntityManager& em)
{
	unsigned defId = params.shorts[0];
	unsigned priority = params.shorts[1];
	const SoundSetDef& def = GameDatabase::instance->SoundSetDefs[defId];
	em.SoundSystem.PlayWorldSound(def, em.GetPosition(id), priority);
}

static void SpawnSprite(const InstructionParams& params, EntityId id, EntityManager& em)
{
	unsigned defId = params.shorts[0];
	const SpriteDef& def = GameDatabase::instance->SpriteDefs[defId];
	int8_t depth = em.DrawSystem.GetComponent(id).depth;

	em.AnimationSystem.RegisterSpawnSprite(def, em.GetPosition(id), depth);
}

static void SpawnSpriteBackground(const InstructionParams& params, EntityId id, EntityManager& em)
{
	unsigned defId = params.shorts[0];
	const SpriteDef& def = GameDatabase::instance->SpriteDefs[defId];
	int8_t depth = em.DrawSystem.GetComponent(id).depth;

	em.AnimationSystem.RegisterSpawnSprite(def, em.GetPosition(id), depth - 1);
}

static void Destroy(const InstructionParams& params, EntityId id, EntityManager& em)
{
	em.AnimationSystem.RegisterDestroy(id);
}

static void Move(const InstructionParams& params, EntityId id, EntityManager& em)
{
	Vector2Int16 pos = em.GetPosition(id);
	uint16_t distance = params.shorts[0];
	pos += Vector2Int16(movementTable32[em.GetOrientation(id)] * distance);
	em.SetPosition(id, pos);
}

static void EnableAI(const InstructionParams& params, EntityId id, EntityManager& em)
{
	em.UnitSystem.GetAIComponent(id).disable = !params.bytes[0];
}


// ======================================================================================

static InstructionAction instructionMap[] =
{
	Frame, Wait, WaitRandom, Face, TurnCW, TurnCCW, GoTo, GoToRandom, Attack, PlaySound, SpawnSprite, SpawnSpriteBackground, Destroy,
	Move, EnableAI
};

void AnimationPlayer::RunAnimation(AnimationComponent& anim, EntityId id, EntityManager& em)
{
	bool running = true;

	while (anim.wait == 0 &&
		(running = anim.instructionCounter < anim.instructionEnd))
	{
		const auto& instr = instructionCache[anim.instructionCounter];
		const auto& action = instructionMap[instr.id];
		action(instr.params, id, em);

		++anim.instructionCounter;
	}

	anim.done = !running;
	//if (state.instructionId == anim.instructionEnd)
	//	state.instructionId = anim.instructionStart;
}