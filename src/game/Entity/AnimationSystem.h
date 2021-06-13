#pragma once

#include "../Span.h"
#include "../MathLib.h"
#include "Component.h"
#include <vector>

struct AnimInstructionDef;

class EntityManager;

class AnimationSystem {

	struct AnimationData {
		std::vector<EntityId> entities;
		std::vector<AnimationComponent> components;
		std::vector<AnimationStateComponent*> states;

		inline size_t size() const { return entities.size(); }
		inline void clear() {
			entities.clear();
			components.clear();
			states.clear();
		}
	};
private:
	AnimationData animData;
public:
	void TickAnimations(EntityManager& em);

	void RunAnimations(EntityManager& em);

	static void BuildInstructionCache(const Span<AnimInstructionDef> instructions);
};