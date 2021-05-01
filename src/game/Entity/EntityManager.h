#pragma once

#include <vector>
#include "Entity.h"
#include "../Span.h"
#include <array>
#include <vector>
#include "RenderSystem.h"
#include "AnimationSystem.h"
#include "../Camera.h"

class EntityManager {

private:
	std::array<Entity, Entity::MaxEntities> entityBuffer;
	std::vector<Entity*> entities;
	std::vector<Entity> collection;
	std::vector<EntityId> deleted;
	unsigned nextFreePos = 0;

	RenderSystem* renderSystem;
	AnimationSystem* animationSystem;

	inline Entity& GetEntity(EntityId id) { return entityBuffer[EntityIdToIndex(id)]; }

public:
	EntityManager();
	~EntityManager();

	EntityId NewEntity(Vector2Int position);
	void DeleteEntity(EntityId id);

	void UpdateEntities();
	void DrawEntites(const Camera& camera);

	RenderComponent& AddRenderComponent(EntityId id, const Sprite& sprite);
	RenderComponent& GetRenderComponent(EntityId id) { return renderSystem->RenderComponents.GetComponent(id); }

	AnimationComponent& AddAnimationComponent(EntityId id, const AnimationClip* clip);
	AnimationComponent& GetAnimationComponent(EntityId id) { return animationSystem->AnimationComponents.GetComponent(id); }

	bool HasEntity(EntityId id) {
		return GetEntity(id).id != 0;
	}

	void SetPosition(EntityId id, Vector2Int position) {
		Entity& e = GetEntity(id);
		e.position = position;
		e.changed = true;
	}
};