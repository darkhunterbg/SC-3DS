#include "EntityManager.h"
#include "RenderSystem.h"

EntityManager::EntityManager(int capacity) {
	entities.reserve(capacity);
}

EntityId EntityManager::NewEntity(Vector2Int position) {
	Entity e;
	e.id = ++idCounter;
	e.position = position;
	entities.push_back(e);

	return e.id;
}

void EntityManager::UpdateEntities(RenderSystem& r) {
	r.UpdateEntities(Span<Entity>( entities.data(), entities.size() ));
}