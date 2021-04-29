#pragma once

#include <vector>
#include "Entity.h"
#include "../Span.h"
#include "RenderSystem.h"


class EntityManager {

private:
	std::vector<Entity> entities;
	unsigned idCounter = 0;


public:
	EntityManager(int capacity = 5000);
	EntityId NewEntity(Vector2Int position);

	void UpdateEntities(RenderSystem& r);

	void SetRenderComponent(EntityId id, RenderComponent* cmp) {
		entities[id - 1].renderComponent = cmp->id;
	}
};