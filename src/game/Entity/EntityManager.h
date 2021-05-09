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

class EntityManager {
private:
	EntityCollection entities;

	RenderSystem renderSystem;
	AnimationSystem animationSystem;
	KinematicSystem kinematicSystem;
	NavigationSystem navigationSystem;

	bool updated = false;

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
		ComponentCollection<MovementComponent> MovementComponents;
		ComponentCollection<NavigationWorkComponent> WorkComponents;
		ComponentCollection<NavigationComponent> NavigationComponents;
		ComponentCollection<OrientationComponent> OrientationComponents;
	} NavigationArchetype;

	struct {
		EntityArchetype Archetype = EntityArchetype("Collision");
		ComponentCollection<ColliderComponent> ColliderComponents;
	} CollisionArchetype;

	ComponentCollection<UnitComponent> UnitComponents;


public:

	EntityManager();
	~EntityManager();

	inline const Span<EntityId> GetEntities() const {
		return entities.GetEntities();
	}
	void DeleteEntity(EntityId id);

	void UpdateEntities();
	void UpdateSecondaryEntities();
	void DrawEntites(const Camera& camera);


	EntityId NewUnit(const UnitDef& def, Vector2Int position, Color color);

	void SetPosition(EntityId e, Vector2Int pos);
	void GoTo(EntityId e, Vector2Int pos);

	bool CollidesWithAny(const Rectangle16& collider, EntityId skip) {

		for (EntityId id : CollisionArchetype.Archetype.GetEntities()) {
			if (id == skip)
				continue;

			int i = Entity::ToIndex(id);

			if(collider.Intersects(CollisionArchetype.ColliderComponents[i].worldCollider))
				return true;
		}

		return false;
	}
};