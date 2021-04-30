#pragma once

#include "../Assets.h"
#include "Entity.h"
#include <vector>

class EntityManager;
class RenderSystem;

struct AnimationComponent {
	static constexpr const int ComponentId = 1;

	const AnimationClip* clip = nullptr;
	int clipFrame = 0;
	int frameCountdown = 0;

	void PlayClip(const AnimationClip* clip) {
		this->clip = clip;
		clipFrame = -1;
		frameCountdown = 1;
	}
};

class AnimationSystem {
	

private:
	std::vector<AnimationComponent> animationComponents;
	int maxComponents = 0;

	int* entityToComponentMap = nullptr;
	std::vector<EntityId> componentToEntityMap;
	std::vector<int> animationUpdates;
public:
	AnimationSystem(int maxEntities);
	~AnimationSystem();

	AnimationComponent& NewComponent(EntityId id, const AnimationClip* clip);
	void RemoveComponent(EntityId id);
	AnimationComponent* GetAnimationComponent(EntityId id) {
		int cid = entityToComponentMap[id - 1];
		if (cid > -1)
			return &animationComponents[cid];
		return nullptr;
	}
	void UpdateAnimations( RenderSystem& renderSystem);
};