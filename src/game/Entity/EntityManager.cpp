#include "EntityManager.h"
#include "RenderSystem.h"
#include "../Platform.h"
#include "../Profiler.h"
#include "../Job.h"

EntityManager::EntityManager() {

	archetypes.push_back(&RenderArchetype.Archetype);
	archetypes.push_back(&AnimationArchetype.Archetype);
	archetypes.push_back(&NavigationArchetype.Archetype);
	archetypes.push_back(&CollisionArchetype.Archetype);
}
EntityManager::~EntityManager() {

}

void EntityManager::Init(Vector2Int16 mapSize)
{
	kinematicSystem.SetSize(mapSize);
}

void EntityManager::DeleteEntity(EntityId id) {
	entities.DeleteEntity(id);
	// TODO: remove archetypes;
}

void EntityManager::CollectEntityChanges() {
	changedData.clear();
	for (EntityId id : entities) {
		int i = Entity::ToIndex(id);
		if (EntityChangeComponents[i].changed) {
			changedData.entity.push_back(id);
			changedData.position.push_back(PositionComponents[i]);

		}
	}

}
void EntityManager::ApplyEntityChanges() {
	for (EntityId id : changedData.entity) {
		int i = Entity::ToIndex(id);

		EntityChangeComponents[i].changed = false;
	}

	for (auto archetype : archetypes)
		archetype->CommitChanges();
}

void EntityManager::UpdateSecondaryEntities() {

	animationSystem.GenerateAnimationUpdates(*this);

	navigationSystem.UpdateNavigation(*this);
}

void EntityManager::UpdateEntities() {

	updated = true;

	navigationSystem.MoveEntities(*this);

	animationSystem.UpdateAnimations();

	CollectEntityChanges();

	renderSystem.UpdatePositions(*this, changedData);
	kinematicSystem.UpdateCollidersPosition(*this, changedData);
	kinematicSystem.ApplyCollidersChange(*this);

	ApplyEntityChanges();
}


void EntityManager::DrawEntites(const Camera& camera) {

	if (!updated)
		return;

	renderSystem.Draw(camera, *this);

	if (DrawColliders)
		kinematicSystem.DrawColliders(camera);
}

EntityId EntityManager::NewUnit(const UnitDef& def, Vector2Int16 position, Color color) {
	EntityId e = entities.NewEntity();
	PositionComponents.NewComponent(e, position);

	EntityChangeComponents.NewComponent(e, { true });
	UnitComponents.NewComponent(e, { &def });

	RenderArchetype.RenderComponents.NewComponent(e, {
		Color4(color),
		def.MovementAnimations[0].GetFrame(0).sprite.image,
		def.MovementAnimationsShadow[0].GetFrame(0).sprite.image,
		def.MovementAnimationsTeamColor[0].GetFrame(0).sprite.image,
		});

	RenderArchetype.OffsetComponents.NewComponent(e, {
	 Vector2Int16(def.MovementAnimations[0].GetFrame(0).offset),
		Vector2Int16(def.MovementAnimationsShadow[0].GetFrame(0).offset)
		});

	RenderArchetype.DestinationComponents.NewComponent(e);
	RenderArchetype.BoundingBoxComponents.NewComponent(e, { {0,0}, def.RenderSize });

	RenderArchetype.Archetype.AddEntity(e);


	auto& a = AnimationArchetype.AnimationComponents.NewComponent(e);
	AnimationArchetype.TrackerComponents.NewComponent(e).PlayClip(&def.MovementAnimations[0]);
	AnimationArchetype.EnableComponents.NewComponent(e).pause = true;

	//a.clip = &def.MovementAnimations[0];
	//a.shadowClip = &def.MovementAnimationsShadow[0];
	//a.unitColorClip = &def.MovementAnimationsTeamColor[0];

	AnimationArchetype.Archetype.AddEntity(e);


	NavigationArchetype.NavigationComponents.NewComponent(e);
	NavigationArchetype.WorkComponents.NewComponent(e, { false });
	NavigationArchetype.MovementComponents.NewComponent(e).SetFromDef(def);
	NavigationArchetype.OrientationComponents.NewComponent(e);

	NavigationArchetype.Archetype.AddEntity(e);

	CollisionArchetype.ColliderComponents.NewComponent(e).collider = def.Collider;
	CollisionArchetype.Archetype.AddEntity(e);

	return e;
}

void EntityManager::SetPosition(EntityId e, Vector2Int16 pos) {
	PositionComponents.GetComponent(e) = pos;
	EntityChangeComponents.GetComponent(e).changed = true;
}
void EntityManager::GoTo(EntityId e, Vector2Int16 pos) {

	NavigationArchetype.WorkComponents.GetComponent(e).work = true;
	NavigationArchetype.NavigationComponents.GetComponent(e).target = pos;
}
