#pragma once

#include "Component.h"
#include <vector>

class EntityManager;


class AnimationSystem {
	struct AnimationData {
		std::vector<AnimationComponent> animation;
		std::vector<AnimationTrackerComponent> tracker;
		std::vector<RenderComponent*> ren;
		std::vector<RenderOffsetComponent*> offset;
		std::vector<EntityChangeComponent*> changed;

		inline size_t size() const { return animation.size(); }
		inline void clear() {
			animation.clear();
			tracker.clear();
			ren.clear();
			offset.clear();
			changed.clear();
		}
	};

private:
	AnimationData data;

	static void UpdateAnimationsJob(int start, int end);
public:
	void GenerateAnimationUpdates(EntityManager& entityManager);
	void UpdateAnimations();

	void SetUnitOrientationAnimations(EntityManager& entityManager);
};