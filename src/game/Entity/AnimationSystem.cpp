#include "AnimationSystem.h"

#include "../Profiler.h"
#include "AnimationPlayer.h"
#include "../Data/AssetDataDefs.h"
#include "EntityManager.h"

void AnimationSystem::RunAnimations(EntityManager& em)
{
	//SectionProfiler p("RunAnimations");

	auto& components = _animComponents.GetComponents();
	auto& entities = _animComponents.GetEntities();

	for (int i = 0; i < components.size(); ++i)
	{
		AnimationComponent& anim = components[i];
		if (anim.wait > 0) --anim.wait;
		if (anim.wait == 0)
		{
			EntityId id = entities[i];
			AnimationPlayer::RunAnimation(anim, id, em);
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
	anim.wait = 0;
	anim.done = false;
	anim.clip = &clip;
}

void AnimationSystem::StartAnimationWithShadow(EntityId id, const AnimClipDef& clip, const Image& shadow)
{
	AnimationComponent& anim = GetComponent(id);
	anim.baseImage = &clip.GetSprite().GetImage();
	anim.shadowImage = &shadow;
	anim.instructionStart = clip.instructionStart;
	anim.instructionEnd = clip.InstructionEnd();
	anim.instructionCounter = clip.instructionStart;
	anim.wait = 0;
	anim.done = false;
	anim.clip = &clip;
}

void AnimationSystem::DeleteEntities(std::vector<EntityId>& entities)
{
	_animComponents.DeleteComponents(entities);
}

size_t AnimationSystem::ReportMemoryUsage()
{
	return _animComponents.GetMemoryUsage();
}

