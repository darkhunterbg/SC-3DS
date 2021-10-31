#include "AnimationPlayer.h"

#include <vector>

#include "EntityManager.h"
#include "../Data/AssetDataDefs.h"
#include "EntityUtil.h"

typedef void(*InstructionAction)(const InstructionParams& params, EntityId id, EntityManager& em);

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
	em.AnimationSystem.GetComponent(id).wait = params.shorts[0] ;
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

static void Attack(const InstructionParams& params, EntityId id, EntityManager& em)
{
	em.UnitSystem.UnitAttackEvent(id);
	//em.GetUnitSystem().UnitAttackEvent(id);
}


// ======================================================================================

static InstructionAction instructionMap[] =
{
	Frame, Wait, WaitRandom, Face, TurnCW, TurnCCW, GoTo, Attack
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