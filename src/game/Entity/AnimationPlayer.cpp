#include "AnimationPlayer.h"

#include <vector>

#include "EntityManager.h"
#include "EntityUtil.h"
#include "../Data/AssetDataDefs.h"

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

	for (const AnimInstructionDef& def : instructions) {
		instructionCache.push_back({ def.param, def.id });
	}
}

// ================================= Instructions =======================================

static void Frame(const InstructionParams& params, EntityId id, EntityManager& em) {
	auto& state = em.AnimationArchetype.StateComponents.GetComponent(id);
	state.animFrame = params.shorts[0];

	EntityUtil::UpdateAnimationVisual(id);
}
static void Wait(const InstructionParams& params, EntityId id, EntityManager& em) {
	em.AnimationArchetype.StateComponents.GetComponent(id).wait =
		params.shorts[0];
}
static void WaitRandom(const InstructionParams& params, EntityId id, EntityManager& em) {

	uint16_t i = (uint16_t)em.rand.Next(params.shorts[0], params.shorts[1]);
	em.AnimationArchetype.StateComponents.GetComponent(id).wait = i;
}
static void Face(const InstructionParams& params, EntityId id, EntityManager& em) {
	EntityUtil::SetOrientation(id, (int)params.bytes[0]);
	EntityUtil::UpdateAnimationVisual(id);
}
static void TurnCW(const InstructionParams& params, EntityId id, EntityManager& em) {
	int orientation = em.OrientationComponents.GetComponent(id);
	EntityUtil::SetOrientation(id, orientation + (int)params.bytes[0]);
	EntityUtil::UpdateAnimationVisual(id);
}
static void TurnCCW(const InstructionParams& params, EntityId id, EntityManager& em) {
	int orientation = em.OrientationComponents.GetComponent(id);
	EntityUtil::SetOrientation(id, orientation - (int)params.bytes[0]);
	EntityUtil::UpdateAnimationVisual(id);
}

static void GoTo(const InstructionParams& params, EntityId id, EntityManager& em) {
	auto& state = em.AnimationArchetype.StateComponents.GetComponent(id);
	const auto& anim = em.AnimationArchetype.AnimationComponents.GetComponent(id);
	state.instructionCounter = anim.instructionStart + params.bytes[0];
	--state.instructionCounter;
}


// ======================================================================================

static InstructionAction instructionMap[] =
{
	Frame, Wait, WaitRandom, Face, TurnCW, TurnCCW, GoTo
};

void AnimationPlayer::RunAnimation(EntityId id, const AnimationComponent& anim, AnimationStateComponent& state, EntityManager& em)
{
	while (state.wait == 0 &&
		state.instructionCounter < anim.instructionEnd)
	{
		const auto& instr = instructionCache[state.instructionCounter];
		const auto& action = instructionMap[instr.id];
		action(instr.params, id, em);

		++state.instructionCounter;
	}

	//if (state.instructionId == anim.instructionEnd)
	//	state.instructionId = anim.instructionStart;
}