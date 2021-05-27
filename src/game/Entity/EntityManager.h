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
#include "PlayerSystem.h"
#include "MapSystem.h"
#include "UnitSystem.h"
#include "SoundSystem.h"
#include "CommandProcessor.h"

#include "../Data/UnitDef.h"
#include "../Camera.h"

struct EntityChangedData {
	std::vector<EntityId> entity;
	std::vector<Vector2Int16> position;
	std::vector<Vector2Int16> oldPosition;

	size_t size() const { return entity.size(); }
	void clear() { entity.clear(); position.clear(); oldPosition.clear(); }
};

class EntityManager {
private:
	EntityManagerCollection entities;

	RenderSystem renderSystem;
	AnimationSystem animationSystem;
	KinematicSystem kinematicSystem;
	NavigationSystem navigationSystem;
	TimingSystem timingSystem;
	PlayerSystem playerSystem;
	MapSystem mapSystem;
	UnitSystem unitSystem;
	SoundSystem soundSystem;
	CommandProcessor commandProcessor;

	bool ready = false;
	bool doneUpdatingVision = false;

	EntityChangedData changedData;

	std::vector<EntityArchetype*> archetypes;
	std::vector<EntityId> scratch;
	std::vector<EntityId> toDelete;
public:

	ComponentCollection<Vector2Int16> OldPositionComponents;
	ComponentCollection<Vector2Int16> PositionComponents;
	ComponentCollection<FlagsComponent> FlagComponents;
	ComponentCollection<uint8_t> OrientationComponents;

	struct {
		EntityArchetype Archetype = EntityArchetype("Parent");
		ComponentCollection<ChildComponent> ChildComponents;
	} ParentArchetype;

	struct {
		EntityArchetype Archetype = EntityArchetype("Render");
		ComponentCollection<RenderComponent> RenderComponents;
		ComponentCollection<Vector2Int16> OffsetComponents;
		ComponentCollection<RenderDestinationComponent> DestinationComponents;
		ComponentCollection<Rectangle16> BoundingBoxComponents;
	} RenderArchetype;

	struct  {
		EntityArchetype Archetype = EntityArchetype("Hidden");
	} HiddenArchetype;

	struct {
		EntityArchetype Archetype = EntityArchetype("Animation");
		ComponentCollection<AnimationComponent> AnimationComponents;
		ComponentCollection<AnimationTrackerComponent> TrackerComponents;

		struct {
			EntityArchetype Archetype = EntityArchetype("Animation.Orientation");
			ComponentCollection< AnimationOrientationComponent> AnimOrientationComponents;
		} OrientationArchetype;
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

			struct {
				EntityArchetype Archetype = EntityArchetype("Unit.Animation.Orientation");
				ComponentCollection<UnitAnimationOrientationComponent> AnimOrientationComponents;
			} OrientationArchetype;

		} AnimationArchetype;


		struct {
			EntityArchetype Archetype = EntityArchetype("Unit.Hidden");
		} HiddenArchetype;

		struct {
			EntityArchetype Archetype = EntityArchetype("Unit.FowVisible");
		} FowVisibleArchetype;


		ComponentCollection<UnitMovementComponent> MovementComponents;
		ComponentCollection<UnitComponent> UnitComponents;
		ComponentCollection<UnitDataComponent> DataComponents;
		ComponentCollection<PlayerId> OwnerComponents;

		ComponentCollection<UnitState> PrevStateComponents;
		ComponentCollection<UnitState> StateComponents;
		ComponentCollection<UnitStateDataComponent> StateDataComponents;

		ComponentCollection< UnitWeaponComponent> WeaponComponents;

		ComponentCollection<UnitAIState> AIStateComponents;
		ComponentCollection<UnitAIStateDataComponent> AIStateDataComponents;
		ComponentCollection<UnitHealthComponent> HealthComponents;
	} UnitArchetype;


	struct {
		EntityArchetype Archetype = EntityArchetype("Timing");
		ComponentCollection<TimingComponent> TimingComponents;
		ComponentCollection<TimingActionComponent> ActionComponents;
	} TimingArchetype;

	struct {
		EntityArchetype Archetype = EntityArchetype("MapObject");
		ComponentCollection<Rectangle16> BoundingBoxComponents;
		ComponentCollection<Rectangle16> DestinationComponents;
		ComponentCollection<uint8_t> MinimapColorId;
	} MapObjectArchetype;
	
	struct {
		EntityArchetype Archetype = EntityArchetype("Sound");
		ComponentCollection<SoundSourceComponent> SourceComponents;
	} SoundArchetype;

private:
	void CollectEntityChanges();
	void ApplyEntityChanges();
	void UpdateChildren();

	void Update0();
	void Update1();
	void Update2();
	void Draw0(const Camera& camera);
	void Draw1(const Camera& camera);
	void Draw2(const Camera& camera);

	unsigned long long frameCounter = 0;
	int8_t updateId = 0;
	uint32_t logicalFrame = 0;

	
public:
	bool DrawColliders = false;
	bool DrawGrid = false;
	bool DrawBoundingBoxes = false;
	bool Muted = false;

	EntityManager();
	~EntityManager();

	void Init(Vector2Int16 mapSize);

	inline PlayerSystem& GetPlayerSystem() {
		return playerSystem;
	}
	inline MapSystem& GetMapSystem() {
		return mapSystem;
	}
	inline SoundSystem& GetSoundSystem() {
		return soundSystem;
	}
	inline KinematicSystem& GetKinematicSystem() {
		return kinematicSystem;
	}
	inline RenderSystem& GetRenderSystem() {
		return renderSystem;
	}
	inline CommandProcessor& GetCommandProcessor() {
		return commandProcessor;
	}
	inline const Span<EntityId> GetEntities() const {
		return entities.GetEntities();
	}
	void DeleteEntity(EntityId id);
	void DeleteEntities(const std::vector<EntityId>& entities, bool sorted = false);
	// NOTE: this will reuse given vector as scratch for performance reasons
	void DeleteEntitiesSorted(std::vector<EntityId>& entities);
	EntityId NewEntity() { return entities.NewEntity(); }
	void NewEntities(unsigned size, std::vector<EntityId>& outIds) {
		entities.NewEntities(size, outIds);
	}
	void ClearEntities();

	void FrameUpdate(const Camera& camera);

	void Draw(const Camera& camera);

	void FullUpdate();

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