#pragma once

#include "IEntitySystem.h"
#include "EntityComponentMap.h"
#include "../MathLib.h"

class EntityManager;
class Camera;

struct ColliderComponent {
	Rectangle16 collider;
};

struct KinematicComponent {
	Vector2Int16 moveOnce;
};

class KinematicSystem : public IEntitySystem {
private:

	EntityComponentMap<ColliderComponent> _colliderComponents;
	EntityComponentMap<KinematicComponent> _kinematicComponents;
	std::vector<Rectangle16> _worldColliders;

public:
	bool ShowColliders = false;

	void NewCollider(EntityId id, const Rectangle16& collider);
	inline bool HasCollider(EntityId id) const
	{
		return _colliderComponents.HasComponent(id);
	}
	void RemoveCollider(EntityId id);
	ColliderComponent& GetCollider(EntityId id) { return _colliderComponents.GetComponent(id); }

	void NewKinematicComponent(EntityId id) { _kinematicComponents.NewComponent(id); }
	KinematicComponent& GetKinematicComponent(EntityId id) { return _kinematicComponents.GetComponent(id); }

	void UpdateColliders(EntityManager& em);
	void Move(EntityManager& em);
	// Inherited via IEntitySystem
	virtual void DeleteEntities(std::vector<EntityId>& entities) override;
	virtual size_t ReportMemoryUsage() override;

	void DrawColliders(const Camera& camera);

	EntityId PointCast(Vector2Int16 point) const;
	void RectCast(const Rectangle16& collider, std::vector<EntityId>& result) const;
	void CircleCast(const Circle16& circle, std::vector<EntityId>& result) const;
	
};