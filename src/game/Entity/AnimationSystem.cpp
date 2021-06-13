#include "AnimationSystem.h"
#include "../Data/AssetDataDefs.h"
#include "EntityManager.h"
#include "EntityUtil.h"

typedef void(*InstructionAction)(const InstructionParams& params, EntityId id, EntityManager& em);

struct AnimInstruction {
	InstructionAction action;
	InstructionParams params;
};


std::vector<AnimInstruction> instructionCache;

void AnimationSystem::TickAnimations(EntityManager& em)
{
	animData.clear();

	for (EntityId id : em.AnimationArchetype.Archetype.GetEntities()) {
		const FlagsComponent& flags = em.FlagComponents.GetComponent(id);
		if (flags.test(ComponentFlags::AnimationEnabled)) {
			AnimationStateComponent& state = em.AnimationArchetype.StateComponents.GetComponent(id);

			if (state.wait > 0)
				--state.wait;


			if (state.wait == 0) {
				animData.entities.push_back(id);

				animData.components.push_back(em.AnimationArchetype.AnimationComponents.GetComponent(id));

				animData.states.push_back(&em.AnimationArchetype.StateComponents.GetComponent(id));
			}
		}
	}
}

void AnimationSystem::RunAnimations(EntityManager& em)
{
	int start = 0;
	int end = animData.size();

	for (int i = start; i < end; ++i) {
		EntityId id = animData.entities[i];

		auto& state = *animData.states[i];
		const auto& anim = animData.components[i];


		while (state.wait == 0 &&
			state.instructionId < anim.instructionEnd)
		{
			const auto& instr = instructionCache[state.instructionId];
			instr.action(instr.params, id, em);

			++state.instructionId;
		}

		if (state.instructionId == anim.instructionEnd)
			state.instructionId = anim.instructionStart;
	}
}


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

	uint16_t size = params.shorts[1] - params.shorts[0];
	uint16_t i = (std::rand() % size) + params.shorts[1];
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

static InstructionAction instructionMap[] =
{
	Frame, Wait, WaitRandom, Face, TurnCW, TurnCCW
};

void AnimationSystem::BuildInstructionCache(const Span<AnimInstructionDef> instructions)
{
	instructionCache.clear();
	instructionCache.reserve(instructions.Size());

	for (const AnimInstructionDef& def : instructions) {
		const auto& action = instructionMap[def.id];
		instructionCache.push_back({ action, def.param });
	}

}
