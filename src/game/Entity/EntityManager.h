#pragma once

#include <vector>
#include <array>

#include "Entity.h"
#include "Archetype.h"
#include "../Span.h"

#include "RenderSystem.h"
#include "AnimationSystem.h"
#include "KinematicSystem.h"
#include "NavigationSystem.h"

#include "../Data/UnitDef.h"
#include "../Camera.h"

struct EntityChangedData {
	std::vector<EntityId> entity;
	std::vector<Vector2Int16> position;

	size_t size() const { return entity.size(); }
	void clear() { entity.clear(); position.clear(); }
};

class EntityManager {
private:
	EntityCollection entities;

	RenderSystem renderSystem;
	AnimationSystem animationSystem;
	KinematicSystem kinematicSystem;
	NavigationSystem navigationSystem;

	bool updated = false;

	EntityChangedData changedData;

	std::vector<EntityArchetype*> archetypes;
public:

	ComponentCollection<Vector2Int16> PositionComponents;
	ComponentCollection<EntityChangeComponent> EntityChangeComponents;

	struct {
		EntityArchetype Archetype = EntityArchetype("Render");
		ComponentCollection<RenderComponent> RenderComponents;
		ComponentCollection<RenderOffsetComponent> OffsetComponents;
		ComponentCollection<RenderDestinationComponent> DestinationComponents;
		ComponentCollection<Rectangle16> BoundingBoxComponents;
	} RenderArchetype;

	struct {
		EntityArchetype Archetype = EntityArchetype("Animation");
		ComponentCollection<AnimationComponent> AnimationComponents;
		ComponentCollection<AnimationEnableComponent> EnableComponents;
		ComponentCollection<AnimationTrackerComponent> TrackerComponents;
	} AnimationArchetype;

	struct {
		EntityArchetype Archetype = EntityArchetype("Navigation");

		ComponentCollection<NavigationWorkComponent> WorkComponents;
		ComponentCollection<NavigationComponent> NavigationComponents;

	} NavigationArchetype;

	struct {
		EntityArchetype Archetype = EntityArchetype("Collision");
		ComponentCollection<ColliderComponent> ColliderComponents;
	} CollisionArchetype;

	struct {
		EntityArchetype Archetype = EntityArchetype("Movement");
		ComponentCollection<MovementComponent> MovementComponents;
	} MovementArchetype;

	struct {
		EntityArchetype Archetype = EntityArchetype("Unit");
		ComponentCollection<OrientationComponent> OrientationComponents;
		ComponentCollection<UnitComponent> UnitComponents;
	} UnitArchetype;

private:
	void CollectEntityChanges();
	void ApplyEntityChanges();

public:
	bool DrawColliders = false;


	EntityManager();
	~EntityManager();

	void Init(Vector2Int16 mapSize);

	inline const Span<EntityId> GetEntities() const {
		return entities.GetEntities();
	}
	void DeleteEntity(EntityId id);

	void UpdateEntities();
	void UpdateSecondaryEntities();
	void DrawEntites(const Camera& camera);


	EntityId NewUnit(const UnitDef& def, Vector2Int16 position, Color color);

	void SetPosition(EntityId e, Vector2Int16 pos);
	void GoTo(EntityId e, Vector2Int16 pos);

	inline bool CollidesWithAny(const Rectangle16& collider, EntityId skip) {

		return kinematicSystem.CollidesWithAny(collider, skip);
	}
	inline void RectCast(const Rectangle16& collider, std::vector<EntityId>& result) {
		return kinematicSystem.RectCast(collider, result);
	}
	inline EntityId PointCast(Vector2Int16 point) {
		return kinematicSystem.PointCast(point);
	}
};