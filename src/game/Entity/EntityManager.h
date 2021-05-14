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
#include "TimingSystem.h"

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
	TimingSystem timingSystem;

	bool updated = false;

	EntityChangedData changedData;

	std::vector<EntityArchetype*> archetypes;
public:

	ComponentCollection<Vector2Int16> PositionComponents;
	ComponentCollection<FlagsComponent> FlagComponents;

	struct {
		EntityArchetype Archetype = EntityArchetype("Render");
		ComponentCollection<RenderComponent> RenderComponents;
		ComponentCollection<Vector2Int16> OffsetComponents;
		ComponentCollection<Vector2Int16> DestinationComponents;
		ComponentCollection<Rectangle16> BoundingBoxComponents;
	} RenderArchetype;


	struct {
		EntityArchetype Archetype = EntityArchetype("Animation");
		ComponentCollection<AnimationComponent> AnimationComponents;
		ComponentCollection<AnimationTrackerComponent> TrackerComponents;
	} AnimationArchetype;

	struct {
		EntityArchetype Archetype = EntityArchetype("Navigation");
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

		struct {
			EntityArchetype Archetype = EntityArchetype("Unit.Render");
			ComponentCollection<RenderUnitComponent> RenderComponents;
			ComponentCollection<RenderUnitOffsetComponent> OffsetComponents;
			ComponentCollection<RenderUnitDestinationComponent> DestinationComponents;
			ComponentCollection<Rectangle16> BoundingBoxComponents;
		}  RenderArchetype;

		struct {
			EntityArchetype Archetype = EntityArchetype("Unit.Animation");
			ComponentCollection<UnitAnimationComponent> AnimationComponents;
			ComponentCollection<UnitAnimationTrackerComponent> TrackerComponents;
		} AnimationArchetype;

		ComponentCollection<uint8_t> OrientationComponents;
		ComponentCollection<UnitMovementComponent> MovementComponents;
		ComponentCollection<UnitComponent> UnitComponents;
	} UnitArchetype;


	struct {
		EntityArchetype Archetype = EntityArchetype("Timing");
		ComponentCollection<TimingComponent> TimingComponents;
		ComponentCollection<TimingActionComponent> ActionComponents;
	} TimingArchetype;
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
	void DeleteEntities(std::vector<EntityId>& entities);
	EntityId NewEntity() { return entities.NewEntity(); }
	void NewEntities(unsigned size, std::vector<EntityId>& outIds) {
		entities.NewEntities(size, outIds);
	}
	void ClearEntities();

	void UpdateEntities();
	void UpdateSecondaryEntities();
	void DrawEntites(const Camera& camera);


	EntityId NewEmptyObject(Vector2Int16 position = { 0,0 });
	EntityId NewUnit(const UnitDef& def, Vector2Int16 position, Color color, EntityId id = Entity::None);
	void PlayAnimation(EntityId id, const AnimationClip& clip);
	void PlayUnitAnimation(EntityId id, const UnitAnimationClip& clip);
	void SetPosition(EntityId e, Vector2Int16 pos);
	void GoTo(EntityId e, Vector2Int16 pos);
	void StartTimer(EntityId e, uint16_t time, TimerExpiredAction action, bool looping = false);

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