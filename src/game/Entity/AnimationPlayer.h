#pragma once
#include "../Span.h"
#include "Entity.h"

struct AnimInstructionDef;
class EntityManager;
struct AnimationComponent;

class AnimationPlayer {
private:
	AnimationPlayer() = delete;
	~AnimationPlayer() = delete;
public:
	static void BuildInstructionCache(const Span<AnimInstructionDef> instructions);	
	static void RunAnimation( AnimationComponent& anim, EntityId id, EntityManager& em);
};