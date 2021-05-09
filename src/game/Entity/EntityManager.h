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

	RenderSystem* renderSystem;
	AnimationSystem* animationSystem;
	KinematicSystem* kinematicSystem;
	NavigationSystem* navigationSystem;

	NavigationArchetype navigationArchetype;
	MovementArchetype movementArchetype;

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


	ComponentCollection<MovementComponent> MovementComponents;
	ComponentCollection<NavigationWorkComponent> NavigationWorkComponents;
	ComponentCollection<NavigationComponent> NavigationComponents;

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
};