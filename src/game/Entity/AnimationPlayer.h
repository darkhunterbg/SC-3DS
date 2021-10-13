#pragma once
#include "../Span.h"

struct AnimInstructionDef;
class EntityManager;
class AnimationComponent;

class AnimationPlayer {
private:
	AnimationPlayer() = delete;
	~AnimationPlayer() = delete;
public:
	static void BuildInstructionCache(const Span<AnimInstructionDef> instructions);	
	static void RunAnimation( AnimationComponent& anim, EntityManager& em);
};