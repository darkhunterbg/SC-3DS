#include "EntityManager.h"
#include "RenderSystem.h"
#include "EntityUtil.h"

#include "../Profiler.h"

#include <algorithm>

static std::vector<EntityId> scratch;

EntityManager::EntityManager() {

	archetypes.push_back(&RenderArchetype.Archetype);
	archetypes.push_back(&AnimationArchetype.Archetype);
	archetypes.push_back(&NavigationArchetype.Archetype);
	archetypes.push_back(&CollisionArchetype.Archetype);
	archetypes.push_back(&MovementArchetype.Archetype);
	archetypes.push_back(&UnitArchetype.Archetype);
	archetypes.push_back(&UnitArchetype.RenderArchetype.Archetype);
	archetypes.push_back(&UnitArchetype.AnimationArchetype.Archetype);
	archetypes.push_back(&TimingArchetype.Archetype);
	archetypes.push_back(&ParentArchetype.Archetype);;
	EntityUtil::emInstance = this;
}
EntityManager::~EntityManager() {
	EntityUtil::emInstance = nullptr;
}

void EntityManager::Init(Vector2Int16 mapSize)
{
	navigationSystem.SetSize(mapSize);
	kinematicSystem.SetSize(mapSize);
}

void EntityManager::DeleteEntity(EntityId id) {
	entities.DeleteEntity(id);
	for (auto& arch : archetypes) {
		if (arch->HasEntity(id))
			arch->RemoveEntity(id);
	}

}
void EntityManager::DeleteEntities(std::vector<EntityId>& e) {

	std::sort(e.begin(), e.end());
	for (auto& arch : archetypes) {
		arch->RemoveEntities(e, true);
	}

	entities.DeleteEntities(e, true);
}
void EntityManager::ClearEntityArchetypes(EntityId id) {
	for (auto& arch : archetypes) {
		if (arch->HasEntity(id))
			arch->RemoveEntity(id);
	}
}
void EntityManager::ClearEntitiesArchetypes(std::vector<EntityId>& e) {
	std::sort(e.begin(), e.end());
	for (auto& arch : archetypes) {
		arch->RemoveEntities(e, true);
	}
}
void EntityManager::ClearEntities()
{
	entities.ClearEntities();
	for (auto& arch : archetypes) {
		arch->ClearEntities();
	}
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

	timingSystem.UpdateTimers(*this);

	//navigationSystem.UpdateNavGrid(*this);

	navigationSystem.UpdateNavigation(*this);

	navigationSystem.ApplyUnitNavigaion(*this);

	animationSystem.SetUnitOrientationAnimations(*this);

	animationSystem.TickAnimations(*this);
}

void EntityManager::UpdateEntities() {

	updated = true;

	timingSystem.ApplyTimerActions(*this);

	kinematicSystem.MoveEntities(*this);

	for (EntityId id : ParentArchetype.Archetype.GetEntities()) {

		if (FlagComponents.GetComponent(id).test(ComponentFlags::PositionChanged)) {
			auto& childComp = ParentArchetype.ChildComponents.GetComponent(id);

			const Vector2Int16& pos = PositionComponents.GetComponent(id);

			for (int i = 0; i < childComp.childCount; ++i) {
				EntityId child = childComp.children[i];

				PositionComponents.GetComponent(child) = pos;
				FlagComponents.GetComponent(child).set(ComponentFlags::PositionChanged);
			}
		}

	}

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

EntityId EntityManager::NewUnit(const UnitDef& def, Vector2Int16 position, Color color, EntityId e) {

	if (e == Entity::None)
		e = entities.NewEntity();
	PositionComponents.NewComponent(e, position);

	FlagComponents.NewComponent(e);
	UnitArchetype.UnitComponents.NewComponent(e, { &def });
	UnitArchetype.OrientationComponents.NewComponent(e);
	UnitArchetype.MovementComponents.NewComponent(e).FromDef(def);
	UnitArchetype.Archetype.AddEntity(e);

	UnitArchetype.RenderArchetype.RenderComponents.NewComponent(e, {
		Color4(color),
		def.Graphics->MovementAnimations[0].GetFrame(0).sprite.image,
		def.Graphics->MovementAnimations[0].GetFrame(0).shadowSprite.image,
		def.Graphics->MovementAnimations[0].GetFrame(0).colorSprite.image,
		});

	UnitArchetype.RenderArchetype.OffsetComponents.NewComponent(e, {
	 Vector2Int16(def.Graphics->MovementAnimations[0].GetFrame(0).offset),
		Vector2Int16(def.Graphics->MovementAnimations[0].GetFrame(0).shadowOffset)
		});

	UnitArchetype.RenderArchetype.DestinationComponents.NewComponent(e);
	UnitArchetype.RenderArchetype.BoundingBoxComponents.NewComponent(e, { {0,0}, def.Graphics->RenderSize });

	UnitArchetype.RenderArchetype.Archetype.AddEntity(e);

	UnitArchetype.AnimationArchetype.AnimationComponents.NewComponent(e);
	UnitArchetype.AnimationArchetype.TrackerComponents.NewComponent(e).PlayClip(&def.Graphics->MovementAnimations[0]);

	UnitArchetype.AnimationArchetype.Archetype.AddEntity(e);

	NavigationArchetype.NavigationComponents.NewComponent(e);

	NavigationArchetype.Archetype.AddEntity(e);

	CollisionArchetype.ColliderComponents.NewComponent(e).collider = def.Graphics->Collider;
	CollisionArchetype.Archetype.AddEntity(e);

	MovementArchetype.MovementComponents.NewComponent(e);
	MovementArchetype.Archetype.AddEntity(e);

	FlagComponents.GetComponent(e).set(ComponentFlags::PositionChanged);
	FlagComponents.GetComponent(e).set(ComponentFlags::UnitRenderChanged);
	FlagComponents.GetComponent(e).set(ComponentFlags::UnitAnimationFrameChanged);


	if (def.Graphics->HasMovementGlow()) {
		auto e2 = NewEntity();
		EntityUtil::SetRenderFromAnimationClip(e2, def.Graphics->MovementGlowAnimations[0], 0);
		EntityUtil::SetPosition(e2, position);
		RenderArchetype.RenderComponents.GetComponent(e2).depth = 1;
		// TODO: anim bounding box
		RenderArchetype.BoundingBoxComponents.NewComponent(e2, { position,{64,64} });
		//RenderArchetype.Archetype.AddEntity(e2);
		AnimationArchetype.Archetype.AddEntity(e2);
		//EntityUtil::PlayAnimation(e2, def.Graphics->MovementGlowAnimations[0]);

		ParentArchetype.Archetype.AddEntity(e);
		ParentArchetype.ChildComponents.NewComponent(e).AddChild(e2);
	}

	return e;
}
void EntityManager::PlayUnitAnimation(EntityId e, const UnitAnimationClip& clip) {
	UnitArchetype.AnimationArchetype.AnimationComponents[e].clip = &clip;
	UnitArchetype.AnimationArchetype.TrackerComponents[e].PlayClip(&clip);
	FlagComponents[e].set(ComponentFlags::AnimationEnabled);
	FlagComponents[e].set(ComponentFlags::UnitAnimationFrameChanged);
}

void EntityManager::GoTo(EntityId e, Vector2Int16 pos) {

	FlagComponents.GetComponent(e).set(ComponentFlags::NavigationWork);
	NavigationArchetype.NavigationComponents.GetComponent(e).target = pos;
	NavigationArchetype.NavigationComponents.GetComponent(e).targetHeading = 255;
}
