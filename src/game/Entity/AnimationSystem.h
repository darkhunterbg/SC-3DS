#pragma once

#include "../Assets.h"
#include "Entity.h"
#include "Component.h"
#include <vector>

class EntityManager;
class RenderSystem;

struct AnimationComponent : public IComponent<1> {

	const AnimationClip* clip = nullptr;
	const AnimationClip* shadowClip = nullptr;

	int clipFrame = 0;
	int frameCountdown = 0;
	bool pause = true;
	void PlayClip(const AnimationClip* clip) {
		this->clip = clip;
		clipFrame = -1;
		frameCountdown = 1;
		pause = false;
	}
};

class AnimationSystem {
	
public:
	ComponentCollection<AnimationComponent> AnimationComponents;
	
	void UpdateAnimations( RenderSystem& renderSystem);
};