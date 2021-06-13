#include "AnimationSystem.h"
#include "../Data/AssetDataDefs.h"
#include "EntityManager.h"

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
			else {
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


		while (state.wait != 0 ||  
			state.instructionId < anim.instructionEnd)
		{
			const auto& instr = instructionCache[state.instructionId];
			instr.action(instr.params, id, em);

			++state.instructionId;
		}
	}
}


static void Frame(const InstructionParams& params, EntityId id, EntityManager& em) {

}
static void Wait(const InstructionParams& params, EntityId id, EntityManager& em) {

}
static void WaitRandom(const InstructionParams& params, EntityId id, EntityManager& em) {

}
static void Face(const InstructionParams& params, EntityId id, EntityManager& em) {

}
static void TurnCW(const InstructionParams& params, EntityId id, EntityManager& em) {

}
static void TurnCCW(const InstructionParams& params, EntityId id, EntityManager& em) {

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
