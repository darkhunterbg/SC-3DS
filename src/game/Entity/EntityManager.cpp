#include "EntityManager.h"
#include "RenderSystem.h"
#include "../Platform.h"
#include "../Profiler.h"
#include "../Job.h"

EntityManager::EntityManager() {
	renderSystem = new RenderSystem();
	animationSystem = new AnimationSystem();
	kinematicSystem = new KinematicSystem();
	navigationSystem = new NavigationSystem();
}
EntityManager::~EntityManager() {
	delete renderSystem;
	delete animationSystem;
	delete kinematicSystem;
	delete navigationSystem;
}

EntityId EntityManager::NewEntity() {

	// This can be speed up with a queue 
	if (entities.size() == Entity::MaxEntities)
		EXCEPTION("No more free entities left!");

	entities.push_back(++lastId);

	return lastId;
}

void EntityManager::DeleteEntity(EntityId id) {
	if (id == Entity::None || id > lastId)
		EXCEPTION("Tried to delete invalid entity!");


	// TODO: this can be speedup with a map

	int totalEntities = entities.size();
	for (unsigned i = 0; i < totalEntities; ++i)
	{
		if (entities[i] == id)
		{
			entities.erase(entities.begin() + i);
			return;
		}
	}

	EXCEPTION("Did not find entity for deletion: %i", id);
}


void EntityManager::UpdateSecondaryEntities() {

	SectionProfiler p("NavCalculate");

	navigationArchetype.clear();

	int navSize = NavigationComponents.size();

	for (int i = 0; i < navSize; ++i) {
		if (NavigationWorkComponents[i].work) {
			navigationArchetype.movement.push_back(MovementComponents[i]);
			navigationArchetype.position.push_back(PositionComponents[i]);
			navigationArchetype.navigation.push_back(&NavigationComponents[i]);
		}
	}

	navigationSystem->UpdateNavigation(navigationArchetype);

	p.Submit();

}


void EntityManager::UpdateEntities() {

	updated = true;

	SectionProfiler p("Movement");

	movementArchetype.clear();

	int navSize = NavigationWorkComponents.size();

	for (int i = 0; i < navSize; ++i) {
		if (NavigationWorkComponents[i].work ){

			movementArchetype.work.push_back(&NavigationWorkComponents[i]);
			movementArchetype.movement.push_back(&MovementComponents[i]);
			movementArchetype.position.push_back(&PositionComponents[i]);
			movementArchetype.navigation.push_back(NavigationComponents[i]);
			movementArchetype.changed.push_back(&EntityChangeComponents[i]);
		}
	}

	navigationSystem->MoveEntities(movementArchetype);


	p.Submit();

	renderUpdatePosArchetype.outPos.clear();
	renderUpdatePosArchetype.worldPos.clear();
	renderUpdatePosArchetype.offset.clear();
	renderUpdatePosArchetype.outBB.clear();

	int size = EntityChangeComponents.size();

	for (int i = 0; i < size; ++i) {
		if (EntityChangeComponents[i].changed) {
			EntityChangeComponents[i].changed = false;

			renderUpdatePosArchetype.outPos.push_back(&RenderDestinationComponents[i]);
			renderUpdatePosArchetype.worldPos.push_back(PositionComponents[i]);
			renderUpdatePosArchetype.offset.push_back(RenderOffsetComponents[i]);
			renderUpdatePosArchetype.outBB.push_back(&RenderBoundingBoxComponents[i]);
		}
	}

	renderSystem->SetRenderPosition(renderUpdatePosArchetype);

	//navigationSystem->UpdateNavigation(entityBuffer.data(), *animationSystem);
	//p.Submit();

	//animationSystem->UpdateAnimations(*renderSystem);

	//collection.clear();

	//for (Entity* e : entities) {
	//	if (e->changed) {
	//		e->changed = false;
	//		collection.push_back(*e);
	//	}
	//}

	//Span<Entity> updated = { collection.data(),collection.size() };

	//kinematicSystem->UpdateEntities(updated);
	//renderSystem->UpdateEntities(updated);

}

void EntityManager::CameraCull(const Camera& camera)
{
	renderArchetype.pos.clear();
	renderArchetype.ren.clear();
	int size = RenderComponents.size();

	Rectangle camRect = camera.GetRectangle();

	for (unsigned i = 0; i < size; ++i) {
		const Rectangle& bb = RenderBoundingBoxComponents[i];

		if (!camRect.Intersects(bb))
			continue;

		renderArchetype.pos.push_back(RenderDestinationComponents[i]);
		renderArchetype.ren.push_back(RenderComponents[i]);
	}
}
void EntityManager::DrawEntites(const Camera& camera) {

	if (!updated)
		return;

	SectionProfiler p("DrawEntities");

	CameraCull(camera);
	renderSystem->Draw(camera, renderArchetype);

	//kinematicSystem->DrawColliders(camera);

	p.Submit();
}

EntityId EntityManager::NewUnit(const UnitDef& def, Vector2Int position, Color color) {
	EntityId e = NewEntity();
	PositionComponents.NewComponent(e, position);
	EntityChangeComponents.NewComponent(e, { true });

	RenderComponents.NewComponent(e, {
		Color4(color),
		def.MovementAnimations[0].GetFrame(0).sprite.image,
		def.MovementAnimationsShadow[0].GetFrame(0).sprite.image,
		def.MovementAnimationsTeamColor[0].GetFrame(0).sprite.image,
		});

	RenderOffsetComponents.NewComponent(e, {
		def.MovementAnimations[0].GetFrame(0).offset,
		def.MovementAnimationsShadow[0].GetFrame(0).offset
		});

	RenderDestinationComponents.NewComponent(e);
	RenderBoundingBoxComponents.NewComponent(e, { {0,0}, def.RenderSize });

	NavigationComponents.NewComponent(e);
	NavigationWorkComponents.NewComponent(e, { false });
	MovementComponents.NewComponent(e, { 0,def.MovementSpeed, def.RotationSpeed });

	//auto& ren = AddRenderComponent(e, def.MovementAnimations[0].GetFrame(0));
	//ren.SetShadowFrame(def.MovementAnimationsShadow[0].GetFrame(0));
	//ren.colorSprite = def.MovementAnimationsTeamColor[0].GetFrame(0).sprite.image;
	//ren.unitColor = Color4(color);

	//AddAnimationComponent(e, &def.MovementAnimations[0]).pause = true;
	//auto& nav = AddNavigationComponent(e, def.RotationSpeed, def.MovementSpeed);
	//AddColliderComponent(e, def.Collider);

	//for (int i = 0; i < 32; ++i) {
	//	nav.clips[i] = (&def.MovementAnimations[i]);
	//	nav.shadowClips[i] = (&def.MovementAnimationsShadow[i]);
	//	nav.colorClips[i] = (&def.MovementAnimationsTeamColor[i]);
	//}

	return e;
}

void EntityManager::SetPosition(EntityId e, Vector2Int pos) {
	PositionComponents.GetComponent(e) = pos;
	EntityChangeComponents.GetComponent(e).changed = true;
}
void EntityManager::GoTo(EntityId e, Vector2Int pos) {
	
	NavigationWorkComponents.GetComponent(e).work = true;
	NavigationComponents.GetComponent(e).target = pos;
}

/*
RenderComponent& EntityManager::AddRenderComponent(EntityId id, const SpriteFrame& frame) {
	Entity& entity = GetEntity(id);
	entity.SetHasComponent<RenderComponent>(true);
	RenderComponent& c = renderSystem->RenderComponents.NewComponent(id);
	c.SetFrame(frame);
	c._dst += entity.position;
	c._shadowDst += entity.position;
	return c;
}
AnimationComponent& EntityManager::AddAnimationComponent(EntityId id, const AnimationClip* clip) {
	AnimationComponent& c = animationSystem->AnimationComponents.NewComponent(id);
	c.PlayClip(clip);
	Entity& entity = GetEntity(id);
	entity.SetHasComponent<AnimationComponent>(true);
	return c;
}
ColliderComponent& EntityManager::AddColliderComponent(EntityId id, const Rectangle& box) {
	Entity& entity = GetEntity(id);
	entity.SetHasComponent<ColliderComponent>(true);
	ColliderComponent& c = kinematicSystem->ColliderComponents.NewComponent(id);
	c.SetBox(box);
	c._worldBox.position += entity.position;
	return c;
}
NavigationComponent& EntityManager::AddNavigationComponent(EntityId id, int turnSpeed, int velocity) {
	Entity& entity = GetEntity(id);
	entity.SetHasComponent<NavigationComponent>(true);
	NavigationComponent& c = navigationSystem->NavigationComponents.NewComponent(id);
	c.turnSpeed = turnSpeed;
	c.velocity = velocity;
	return c;
}
*/