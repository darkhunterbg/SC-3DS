#include "EntityManager.h"
#include "RenderSystem.h"
#include "../Platform.h"
#include "../Profiler.h"
#include "../Job.h"

EntityManager::EntityManager() {

}
EntityManager::~EntityManager() {

}

void EntityManager::DeleteEntity(EntityId id) {
	entities.DeleteEntity(id);
	// TODO: remove components;
}


void EntityManager::UpdateSecondaryEntities() {

	animationSystem.GenerateAnimationUpdates(*this);

	navigationSystem.UpdateNavigation(*this);
}

void EntityManager::UpdateEntities() {

	updated = true;

	navigationSystem.MoveEntities(*this);
	
	animationSystem.UpdateAnimations();

	renderSystem.UpdatePositions(*this);
}


void EntityManager::DrawEntites(const Camera& camera) {

	if (!updated)
		return;

	renderSystem.Draw(camera, *this);

	Rectangle camRect = camera.GetRectangle();
	Color c = Colors::LightGreen;
	c.a = 0.5f;

	//for (EntityId id : CollisionArchetype.Archetype.GetEntities()) {
	//	int i = Entity::ToIndex(id);
	//	const auto& cmp = CollisionArchetype.ColliderComponents[i];
	//	Rectangle dst = { Vector2Int( cmp.worldCollider.position), Vector2Int( cmp.worldCollider.size )};

	//	if (camRect.Intersects(dst)) {

	//		dst.position -= camRect.position;
	//		dst.position /= camera.Scale;
	//		dst.size /= camera.Scale;

	//		Platform::DrawRectangle(dst, c);
	//	}
	//
	//}


	//kinematicSystem->DrawColliders(camera);


}

EntityId EntityManager::NewUnit(const UnitDef& def, Vector2Int position, Color color) {
	EntityId e = entities.NewEntity();
	PositionComponents.NewComponent(e, Vector2Int16( position));
	
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
	RenderArchetype.BoundingBoxComponents.NewComponent(e, { {0,0}, Vector2Int16( def.RenderSize) });

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

	CollisionArchetype.ColliderComponents.NewComponent(e).SetBox(def.Collider);
	CollisionArchetype.Archetype.AddEntity(e);

	return e;
}

void EntityManager::SetPosition(EntityId e, Vector2Int pos) {
	PositionComponents.GetComponent(e) = pos;
	EntityChangeComponents.GetComponent(e).changed = true;
}
void EntityManager::GoTo(EntityId e, Vector2Int pos) {

	NavigationArchetype.WorkComponents.GetComponent(e).work = true;
	NavigationArchetype.NavigationComponents.GetComponent(e).target = pos;
}
