#pragma once

#include "../Entity/Entity.h"
#include "../Assets.h"
#include "../Entity/Common.h"
#include "../Data/RaceDef.h"

class EntityManager;

struct GameHUDContext {

public:
	EntityManager* em = nullptr;
	const RaceDef* race = nullptr;

	EntityCollection selectedEntities;

	PlayerId player = 1;

	inline EntityManager& GetEntityManager() const {
		return *em;
	}

	inline const SpriteFrameAtlas& GetCommandIconsAtlas() const {
		return *race->CommandIconsAtlas;
	}
};