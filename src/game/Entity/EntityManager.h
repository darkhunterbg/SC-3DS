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

	RenderArchetype renderArchetype;
	RenderUpdatePositionArchetype renderUpdatePosArchetype;
	NavigationArchetype navigationArchetype;
	MovementArchetype movementArchetype;
	AnimationArchetype animationArchetype;

	void CameraCull(const Camera& camera);

	bool updated = false;
public:

	ComponentCollection<Vector2Int> PositionComponents;
	ComponentCollection<EntityChangeComponent> EntityChangeComponents;

	ComponentCollection<RenderComponent> RenderComponents;
	ComponentCollection<RenderOffsetComponent> RenderOffsetComponents;
	ComponentCollection<RenderDestinationComponent> RenderDestinationComponents;
	ComponentCollection<Rectangle> RenderBoundingBoxComponents;

	ComponentCollection<MovementComponent> MovementComponents;
	ComponentCollection<NavigationWorkComponent> NavigationWorkComponents;
	ComponentCollection<NavigationComponent> NavigationComponents;

	ComponentCollection<UnitComponent> UnitComponents;

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
};