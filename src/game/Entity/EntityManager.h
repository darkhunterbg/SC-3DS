#pragma once

#include <vector>
#include "Entity.h"
#include "../Span.h"
#include <array>
#include <vector>
#include "RenderSystem.h"
#include "../Camera.h"

class EntityManager {
	static constexpr const int MaxEntities = 5000;
private:
	std::array<Entity, MaxEntities> entityBuffer;
	std::vector<Entity*> entities;
	std::vector<Entity> collection;
	std::vector<EntityId> deleted;
	unsigned nextFreePos = 0;

	RenderSystem* renderSystem;

	inline Entity& GetEntity(EntityId id) { return entityBuffer[id - 1]; }

public:
	EntityManager();
	~EntityManager();

	EntityId NewEntity(Vector2Int position);
	void DeleteEntity(EntityId id);

	void UpdateEntities();
	void DrawEntites(const Camera& camera);

	RenderComponent& AddRenderComponent(EntityId id, const Sprite& sprite);

	bool HasEntity(EntityId id) {
		return entityBuffer[id-1].id != 0;
	}

	void SetPosition(EntityId id, Vector2Int position) {
		Entity& e = GetEntity(id);
		e.position = position;
		e.changed = true;
	}
};