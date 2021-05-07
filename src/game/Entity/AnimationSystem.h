#pragma once

#include "Archetype.h"
#include <vector>

class EntityManager;
class RenderSystem;

class AnimationSystem {
	
public:
	void UpdateAnimations(AnimationArchetype& archetype );
};