#include "EntityManager.h"
#include "RenderSystem.h"
#include "../Platform.h"
#include "../Profiler.h"


EntityManager::EntityManager() {
	renderSystem = new RenderSystem(MaxEntities);
	animationSystem = new AnimationSystem(MaxEntities);
}
EntityManager::~EntityManager() {
	delete renderSystem;
	delete animationSystem;
}

EntityId EntityManager::NewEntity(Vector2Int position) {

	// This can be speed up with a queue 
	bool foundFree = false;
	for (; nextFreePos < MaxEntities; nextFreePos++) {
		if (entityBuffer[nextFreePos].id == 0)
		{
			foundFree = true;
			break;
		}
	}
	if (!foundFree) {
		for (nextFreePos = 0; nextFreePos < MaxEntities; nextFreePos++) {
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
	if (id == 0 || id > MaxEntities)
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
				renderSystem->RemoveComponent(id);

			if (e->HasComponent<AnimationComponent>())
				animationSystem->RemoveComponent(id);

			return;
		}
	}

	EXCEPTION("Did not find entity for deletion: %i", id);
}

void EntityManager::UpdateEntities() {

	SectionProfiler p("UpdateAnimation");

	animationSystem->UpdateAnimations(*renderSystem);

	p.Submit();

	collection.clear();

	for (Entity* e : entities) {
		if (e->changed) {
			e->changed = false;
			collection.push_back(*e);
		}
	}

	Span<Entity> updated = { collection.data(),collection.size() };

	renderSystem->UpdateEntities(updated);

	//p.Submit();
}

void EntityManager::DrawEntites(const Camera& camera) {

	//SectionProfiler p("DrawEntities");

	renderSystem->Draw(camera);

	//p.Submit();
}


RenderComponent& EntityManager::AddRenderComponent(EntityId id, const Sprite& sprite) {

	RenderComponent& c = renderSystem->NewComponent(id, sprite);
	Entity& entity = GetEntity(id);
	entity.changed = true;
	entity.SetHasComponent<RenderComponent>(true);
	return c;
}
AnimationComponent& EntityManager::AddAnimationComponent(EntityId id, const AnimationClip* clip) {
	AnimationComponent& c = animationSystem->NewComponent(id, clip);
	Entity& entity = GetEntity(id);
	entity.SetHasComponent<AnimationComponent>(true);
	return c;
}