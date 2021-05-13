#pragma once

#include "Component.h"
#include <vector>

class EntityManager;


class AnimationSystem {
	struct UnitAnimationData {
		std::vector<UnitAnimationComponent> animation;
		std::vector<UnitAnimationTrackerComponent> tracker;
		std::vector<RenderUnitComponent*> ren;
		std::vector<RenderUnitOffsetComponent*> offset;
		std::vector<FlagsComponent*> flags;

		inline size_t size() const { return animation.size(); }
		inline void clear() {
			animation.clear();
			tracker.clear();
			ren.clear();
			offset.clear();
			flags.clear();
		}
	};

	struct AnimationData {
		std::vector<AnimationComponent> animation;
		std::vector<AnimationTrackerComponent> tracker;
		std::vector<RenderComponent*> ren;
		std::vector<Vector2Int16*> offset;
		std::vector<FlagsComponent*> flags;

		inline size_t size() const { return animation.size(); }
		inline void clear() {
			animation.clear();
			tracker.clear();
			ren.clear();
			offset.clear();
			flags.clear();
		}
	};

private:
	UnitAnimationData unitData;
	AnimationData data;
	static void UpdateUnitAnimationsJob(int start, int end);
	static void UpdateAnimationsJob(int start, int end);
	static void TickAnimationsJob(int start, int end);
	static void TickUnitAnimationsJob(int start, int end);
public:
	void TickAnimations(EntityManager& entityManager);
	void UpdateAnimations();

	void SetUnitOrientationAnimations(EntityManager& entityManager);
};