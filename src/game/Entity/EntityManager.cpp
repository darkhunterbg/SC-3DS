#include "EntityManager.h"
#include "RenderSystem.h"
#include "../Platform.h"
#include "../Profiler.h"


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

EntityId EntityManager::NewEntity(Vector2Int position) {

	// This can be speed up with a queue 
	bool foundFree = false;
	for (; nextFreePos < Entity::MaxEntities; nextFreePos++) {
		if (entityBuffer[nextFreePos].id == 0)
		{
			foundFree = true;
			break;
		}
	}
	if (!foundFree) {
		for (nextFreePos = 0; nextFreePos < Entity::MaxEntities; nextFreePos++) {
			if (entityBuffer[nextFreePos].id == 0)
			{
				foundFree = true;
				break;
			}
		}
	}
	if (!foundFree)
		EXCEPTION("No more free entities left!");

	Entity& e = entityBuffer[nextFreePos++];
	e = Entity();

	e.id = nextFreePos;
	e.position = position;

	entities.push_back(&e);

	return e.id;
}

void EntityManager::DeleteEntity(EntityId id) {
	if (id == 0 || id > Entity::MaxEntities)
		EXCEPTION("Tried to delete invalid entity!");


	// TODO: this can be speedup with a map

	int totalEntities = entities.size();
	for (unsigned i = 0; i < totalEntities; ++i)
	{
		Entity* e = entities[i];

		if (e->id == id)
		{
			entities.erase(entities.begin() + i);
			e->id = 0;

			if (e->HasComponent<RenderComponent>())
				renderSystem->RenderComponents.RemoveComponent(id);
			if (e->HasComponent<AnimationComponent>())
				animationSystem->AnimationComponents.RemoveComponent(id);
			if (e->HasComponent<ColliderComponent>())
				kinematicSystem->ColliderComponents.RemoveComponent(id);
			if (e->HasComponent<NavigationComponent>())
				navigationSystem->NavigationComponents.RemoveComponent(id);

			return;
		}
	}

	EXCEPTION("Did not find entity for deletion: %i", id);
}

void EntityManager::UpdateEntities() {

	//SectionProfiler p("Navigation");

	navigationSystem->UpdateNavigation(entityBuffer.data(), *animationSystem);
	//p.Submit();

	animationSystem->UpdateAnimations(*renderSystem);

	collection.clear();

	for (Entity* e : entities) {
		if (e->changed) {
			e->changed = false;
			collection.push_back(*e);
		}
	}

	Span<Entity> updated = { collection.data(),collection.size() };

	kinematicSystem->UpdateEntities(updated);
	renderSystem->UpdateEntities(updated);

	//p.Submit();
}

void EntityManager::DrawEntites(const Camera& camera) {

	//SectionProfiler p("DrawEntities");

	renderSystem->Draw(camera);

	//kinematicSystem->DrawColliders(camera);

	//p.Submit();
}

EntityId EntityManager::NewUnit(const UnitDef& def, Vector2Int position, Color color) {
	EntityId e = NewEntity(position);
	auto& ren = AddRenderComponent(e, def.MovementAnimations[0].GetFrame(0));
	ren.SetShadowFrame(def.MovementAnimationsShadow[0].GetFrame(0));
	ren.colorSprite = def.MovementAnimationsTeamColor[0].GetFrame(0).sprite;
	ren.unitColor = color;

	AddAnimationComponent(e, &def.MovementAnimations[0]).pause = true;
	auto& nav = AddNavigationComponent(e, def.RotationSpeed, def.MovementSpeed);
	AddColliderComponent(e, def.Collider);

	for (int i = 0; i < 32; ++i) {
		nav.clips[i] = (&def.MovementAnimations[i]);
		nav.shadowClips[i] = (&def.MovementAnimationsShadow[i]);
		nav.colorClips[i] = (&def.MovementAnimationsTeamColor[i]);
	}

	return e;
}

RenderComponent& EntityManager::AddRenderComponent(EntityId id, const SpriteFrame& frame) {
	Entity& entity = GetEntity(id);
	entity.SetHasComponent<RenderComponent>(true);
	RenderComponent& c = renderSystem->RenderComponents.NewComponent(id);
	c.SetFrame(frame);
	c._dst.position += entity.position;
	c._shadowDst.position += entity.position;
	renderSystem->archetypes.push_back({ c._dst });
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