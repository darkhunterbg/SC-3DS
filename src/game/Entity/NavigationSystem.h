#pragma once

#include "Entity.h"
#include "Component.h"

class EntityManager;
class AnimationSystem;
class AnimationClip;

struct NavigationComponent  {
	bool work = false;
	bool newNav = false;
	Vector2Int target = { 0,0 };

	int turnSpeed = 0;
	int velocity = 0;

	const AnimationClip* clips[32];
	const AnimationClip* shadowClips[32];
	const AnimationClip* colorClips[32];

	void GoTo(Vector2Int position) {
		target = position;
		work = true;
		newNav = true;
	}

};

class NavigationSystem {
public:
	ComponentCollection<NavigationComponent> NavigationComponents;

	void UpdateNavigation(Entity* entities, AnimationSystem& animationSystem);
};