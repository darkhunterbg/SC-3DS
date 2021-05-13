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
	archetypes.push_back(&UnitArchetype.RenderArchetype.Archetype);
	archetypes.push_back(&UnitArchetype.AnimationArchetype.Archetype);
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
		if (FlagComponents[i].test(ComponentFlags::PositionChanged)) {
			changedData.entity.push_back(id);
			changedData.position.push_back(PositionComponents[i]);
		}
	}
}
void EntityManager::ApplyEntityChanges() {
	for (EntityId id : changedData.entity) {
		int i = Entity::ToIndex(id);
		FlagComponents[i].clear(ComponentFlags::PositionChanged);
	}

	for (auto archetype : archetypes)
		archetype->CommitChanges();
}

void EntityManager::UpdateSecondaryEntities() {

	//navigationSystem.UpdateNavGrid(*this);

	navigationSystem.UpdateNavigation(*this);

	navigationSystem.ApplyUnitNavigaion(*this);

	animationSystem.SetUnitOrientationAnimations(*this);

	animationSystem.TickAnimations(*this);
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

	FlagComponents.NewComponent(e);
	UnitArchetype.UnitComponents.NewComponent(e, { &def });
	UnitArchetype.OrientationComponents.NewComponent(e);
	UnitArchetype.MovementComponents.NewComponent(e).FromDef(def);
	UnitArchetype.Archetype.AddEntity(e);

	UnitArchetype.RenderArchetype.RenderComponents.NewComponent(e, {
		Color4(color),
		def.MovementAnimations[0].GetFrame(0).sprite.image,
		def.MovementAnimations[0].GetFrame(0).shadowSprite.image,
		def.MovementAnimations[0].GetFrame(0).colorSprite.image,
		});

	UnitArchetype.RenderArchetype.OffsetComponents.NewComponent(e, {
	 Vector2Int16(def.MovementAnimations[0].GetFrame(0).offset),
		Vector2Int16(def.MovementAnimations[0].GetFrame(0).shadowOffset)
		});

	UnitArchetype.RenderArchetype.DestinationComponents.NewComponent(e);
	UnitArchetype.RenderArchetype.BoundingBoxComponents.NewComponent(e, { {0,0}, def.RenderSize });

	UnitArchetype.RenderArchetype.Archetype.AddEntity(e);

	UnitArchetype.AnimationArchetype.AnimationComponents.NewComponent(e);
	UnitArchetype.AnimationArchetype.TrackerComponents.NewComponent(e).PlayClip(&def.MovementAnimations[0]);

	UnitArchetype.AnimationArchetype.Archetype.AddEntity(e);

	NavigationArchetype.NavigationComponents.NewComponent(e);

	NavigationArchetype.Archetype.AddEntity(e);

	CollisionArchetype.ColliderComponents.NewComponent(e).collider = def.Collider;
	CollisionArchetype.Archetype.AddEntity(e);

	MovementArchetype.MovementComponents.NewComponent(e);
	MovementArchetype.Archetype.AddEntity(e);

	FlagComponents.GetComponent(e).set(ComponentFlags::PositionChanged);
	FlagComponents.GetComponent(e).set(ComponentFlags::UnitRenderChanged);
	FlagComponents.GetComponent(e).set(ComponentFlags::AnimationFrameChanged);

	return e;
}
void EntityManager::PlayAnimation(EntityId e, const AnimationClip& clip) {
	AnimationArchetype.AnimationComponents[e].clip = &clip;
	AnimationArchetype.TrackerComponents[e].PlayClip(&clip);
	FlagComponents[e].set(ComponentFlags::AnimationEnabled);
	FlagComponents[e].set(ComponentFlags::AnimationFrameChanged);

}
void EntityManager::PlayUnitAnimation(EntityId e, const UnitAnimationClip& clip) {
	UnitArchetype.AnimationArchetype.AnimationComponents[e].clip = &clip;
	UnitArchetype.AnimationArchetype.TrackerComponents[e].PlayClip(&clip);
	FlagComponents[e].set(ComponentFlags::AnimationEnabled);
	FlagComponents[e].set(ComponentFlags::UnitAnimationFrameChanged);
}
void EntityManager::SetPosition(EntityId e, Vector2Int16 pos) {
	PositionComponents.GetComponent(e) = pos;
	FlagComponents[e].set(ComponentFlags::PositionChanged);
}
void EntityManager::GoTo(EntityId e, Vector2Int16 pos) {

	FlagComponents.GetComponent(e).set(ComponentFlags::NavigationWork);
	NavigationArchetype.NavigationComponents.GetComponent(e).target = pos;
	NavigationArchetype.NavigationComponents.GetComponent(e).targetHeading = 255;
}
