#pragma once
#include "../Span.h"
#include "Component.h"

struct AnimInstructionDef;
class EntityManager;

class AnimationPlayer {
private:
	AnimationPlayer() = delete;
	~AnimationPlayer() = delete;
public:
	static void BuildInstructionCache(const Span<AnimInstructionDef> instructions);	
	static void RunAnimation(EntityId id,const AnimationComponent& anim, AnimationStateComponent& state, EntityManager& em);
};