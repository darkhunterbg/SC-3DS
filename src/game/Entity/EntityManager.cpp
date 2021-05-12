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
	archetypes.push_back(&MovementArchetype.Archetype);
	archetypes.push_back(&UnitArchetype.Archetype);
}
EntityManager::~EntityManager() {

}

void EntityManager::Init(Vector2Int16 mapSize)
{
	navigationSystem.SetSize(mapSize);
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

	//navigationSystem.UpdateNavGrid(*this);

	navigationSystem.UpdateNavigation(*this);

	navigationSystem.ApplyUnitNavigaion(*this);

	animationSystem.SetUnitOrientationAnimations(*this);

	animationSystem.GenerateAnimationUpdates(*this);
}

void EntityManager::UpdateEntities() {

	updated = true;

	kinematicSystem.MoveEntities(*this);

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
	UnitArchetype.UnitComponents.NewComponent(e, { &def });
	UnitArchetype.OrientationComponents.NewComponent(e);
	UnitArchetype.MovementComponents.NewComponent(e).FromDef(def);
	UnitArchetype.Archetype.AddEntity(e);

	RenderArchetype.RenderComponents.NewComponent(e, {
		Color4(color),
		def.MovementAnimations[0].GetFrame(0).sprite.image,
		def.MovementAnimations[0].GetFrame(0).shadowSprite.image,
		def.MovementAnimations[0].GetFrame(0).colorSprite.image,
		});

	RenderArchetype.OffsetComponents.NewComponent(e, {
	 Vector2Int16(def.MovementAnimations[0].GetFrame(0).offset),
		Vector2Int16(def.MovementAnimations[0].GetFrame(0).shadowOffset)
		});

	RenderArchetype.DestinationComponents.NewComponent(e);
	RenderArchetype.BoundingBoxComponents.NewComponent(e, { {0,0}, def.RenderSize });

	RenderArchetype.Archetype.AddEntity(e);


	auto& a = AnimationArchetype.AnimationComponents.NewComponent(e);
	AnimationArchetype.TrackerComponents.NewComponent(e).PlayClip(&def.MovementAnimations[0]);
	AnimationArchetype.EnableComponents.NewComponent(e).pause = true;

	AnimationArchetype.Archetype.AddEntity(e);

	NavigationArchetype.NavigationComponents.NewComponent(e);
	NavigationArchetype.WorkComponents.NewComponent(e, { false });


	NavigationArchetype.Archetype.AddEntity(e);

	CollisionArchetype.ColliderComponents.NewComponent(e).collider = def.Collider;
	CollisionArchetype.Archetype.AddEntity(e);

	MovementArchetype.MovementComponents.NewComponent(e);
	MovementArchetype.Archetype.AddEntity(e);

	return e;
}

void EntityManager::SetPosition(EntityId e, Vector2Int16 pos) {
	PositionComponents.GetComponent(e) = pos;
	EntityChangeComponents.GetComponent(e).changed = true;
}
void EntityManager::GoTo(EntityId e, Vector2Int16 pos) {

	NavigationArchetype.WorkComponents.GetComponent(e).work = true;
	NavigationArchetype.NavigationComponents.GetComponent(e).target = pos;
	NavigationArchetype.NavigationComponents.GetComponent(e).targetHeading = 255;
}
