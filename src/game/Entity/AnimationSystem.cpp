#include "AnimationSystem.h"

#include "../Profiler.h"
#include "AnimationPlayer.h"
#include "../Data/AssetDataDefs.h"
#include "EntityManager.h"

void AnimationSystem::RunAnimations(EntityManager& em)
{
	SectionProfiler p("RunAnimations");

	for (AnimationComponent& anim : _animComponents)
	{
		if (anim.wait > 0) --anim.wait;
		if (anim.wait == 0)
		{
			AnimationPlayer::RunAnimation(anim, em);
		}
	}
}

void AnimationSystem::StartAnimation(EntityId id, const AnimClipDef& clip)
{
	AnimationComponent& anim = GetComponent(id);
	anim.baseImage = &clip.GetSprite().GetImage();
	anim.shadowImage = nullptr;
	anim.instructionStart = clip.instructionStart;
	anim.instructionEnd = clip.InstructionEnd();
	anim.instructionCounter = clip.instructionStart;
	anim.animFrame = 0;
	anim.wait = 0;
	anim.done = false;
	anim.entityId = id;
}

void AnimationSystem::StartAnimationWithShadow(EntityId id, const AnimClipDef& clip, const Image& shadow)
{
	AnimationComponent& anim = GetComponent(id);
	anim.baseImage = &clip.GetSprite().GetImage();
	anim.shadowImage = &shadow;
	anim.instructionStart = clip.instructionStart;
	anim.instructionEnd = clip.InstructionEnd();
	anim.instructionCounter = clip.instructionStart;
	anim.animFrame = 0;
	anim.wait = 0;
	anim.done = false;
	anim.entityId = id;
}

