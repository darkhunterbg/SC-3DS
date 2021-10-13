#pragma once

#include "../Assets.h"
#include "Entity.h"

class EntityManager;
class AnimClipDef;

struct AnimationComponent {
	const Image* baseImage;
	const Image* shadowImage = nullptr;
	uint32_t instructionStart;
	uint32_t instructionEnd;

	int32_t instructionCounter = 0;
	uint16_t animFrame = 0;
	uint8_t wait;
	bool done = false;

	EntityId entityId;
};

class AnimationSystem {

	std::vector<AnimationComponent> _animComponents;

	std::array<short, Entity::MaxEntities> _entityToIndexMap;
	EntityId _maxEntity = Entity::None;


	

public:
	void RunAnimations(EntityManager& em);
	void NewComponent(EntityId id)
	{
		short index = _animComponents.size();

		_animComponents.push_back(AnimationComponent());
		_entityToIndexMap[Entity::ToIndex(id)] = index;

		if (id > _maxEntity) _maxEntity = id;
	}

	AnimationComponent& GetComponent(EntityId id)
	{
		short index = _entityToIndexMap[Entity::ToIndex(id)];
		return _animComponents[index];
	}

	void StartAnimation(EntityId id, const AnimClipDef& clip);
	void StartAnimationWithShadow(EntityId id, const AnimClipDef& clip, const Image& shadow);
};