#pragma once

#include "Archetype.h"


#include <vector>

class EntityManager;

struct AnimationArchetype {
	ArchetypeCollection<AnimationComponent> animation;
	ArchetypeCollection<AnimationTrackerComponent> tracker;
	ArchetypeCollection<RenderComponent*> ren;
	ArchetypeCollection<RenderOffsetComponent*> offset;
	ArchetypeCollection<EntityChangeComponent*> changed;

	inline size_t size() const { return animation.size(); }
	inline void clear() {
		animation.clear();
		tracker.clear();
		ren.clear();
		offset.clear();
		changed.clear();
	}
};

class AnimationSystem {
	AnimationArchetype archetype;
public:
	void GenerateAnimationUpdates(EntityManager& entityManager);
	void UpdateAnimations( );
};