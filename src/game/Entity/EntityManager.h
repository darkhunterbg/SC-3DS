#pragma once

#include <vector>
#include "Entity.h"
#include "../Span.h"
#include <array>
#include <vector>
#include "RenderSystem.h"
#include "AnimationSystem.h"
#include "KinematicSystem.h"

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
	KinematicSystem* kinematicSystem;

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

	ColliderComponent& AddColliderComponent(EntityId id, const Rectangle& box);
	ColliderComponent& GetColliderComponent(EntityId id) { return kinematicSystem->ColliderComponents.GetComponent(id); }
	void RemoveColliderComponent(EntityId id) {
		kinematicSystem->ColliderComponents.RemoveComponent(id);
	}

	
	inline void PointCast(Vector2Int point, std::vector<EntityId>& result) {
		kinematicSystem->PointCast(point, result);
	}
	inline EntityId PointCast(Vector2Int point) {
		return kinematicSystem->PointCast(point);
	}
	inline void RectCast(const Rectangle& rect, std::vector<EntityId>& result) {
		kinematicSystem->RectCast(rect, result);
	}

	bool HasEntity(EntityId id) {
		return GetEntity(id).id != 0;
	}

	void SetPosition(EntityId id, Vector2Int position) {
		Entity& e = GetEntity(id);
		e.position = position;
		e.changed = true;
	}
};