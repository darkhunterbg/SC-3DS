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
	std::vector<EntityId> entities;
	std::vector<EntityId> deleted;
	EntityId lastId = Entity::None;

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

	ComponentCollection<RenderComponent> RenderComponents;
	ComponentCollection<RenderOffsetComponent> RenderOffsetComponents;
	ComponentCollection<RenderDestinationComponent> RenderDestinationComponents;
	ComponentCollection<Rectangle16> RenderBoundingBoxComponents;

	ComponentCollection<MovementComponent> MovementComponents;
	ComponentCollection<NavigationWorkComponent> NavigationWorkComponents;
	ComponentCollection<NavigationComponent> NavigationComponents;

	ComponentCollection<UnitComponent> UnitComponents;

	ComponentCollection<AnimationEnableComponent> AnimationEnableComponents;
	ComponentCollection<AnimationTrackerComponent> AnimationTrackerComponents;
	ComponentCollection<AnimationComponent> AnimationComponents;

	EntityManager();
	~EntityManager();

	EntityId NewEntity();
	void DeleteEntity(EntityId id);

	void UpdateEntities();
	void UpdateSecondaryEntities();
	void DrawEntites(const Camera& camera);


	EntityId NewUnit(const UnitDef& def, Vector2Int position, Color color);

	void SetPosition(EntityId e, Vector2Int pos);
	void GoTo(EntityId e, Vector2Int pos);
	void SetOrientation(EntityId e, unsigned orientation);
};