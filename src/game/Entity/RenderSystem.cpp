#include "RenderSystem.h"
#include "../Platform.h"
#include "../Profiler.h"
#include <cstring>

RenderSystem::RenderSystem(int maxEntities) {
	entityToComponentMap = new int[maxEntities];
	memset(entityToComponentMap, 0, sizeof(maxEntities) * sizeof(int));
	this->maxComponents = maxEntities;
}
RenderSystem::~RenderSystem() {
	delete[] entityToComponentMap;
}

void RenderSystem::Draw(const Camera& camera) {
	Rectangle camRect = camera.GetRectangle();

	for(RenderComponent& cmp : renderComponents)
	{
		if (!camRect.Intersects(cmp._dst))
			continue;

		Rectangle dst = cmp._dst;
		dst.position -= camRect.position;
		dst.position /= camera.Scale;
		dst.size /= camera.Scale;

		Platform::Draw(cmp.sprite, dst, Colors::White);
	}
}

void RenderSystem::UpdateEntities(const Span<Entity> entities) {
	//SectionProfiler p("RenderSystem::UpdateEntities");

	// Only for ones that moved

	for (const Entity& entity : entities) {
		int id = entityToComponentMap[entity.BufferPosition()];
		RenderComponent& c = renderComponents[id];

		c._dst = c.sprite.rect;
		c._dst.position = entity.position;
	}

	//p.Submit();
}

RenderComponent& RenderSystem::NewComponent(EntityId id,const Sprite& sprite) {

	int cid = renderComponents.size();
	entityToComponentMap[id - 1] = cid;
	componentToEntityMap.push_back(id );
	renderComponents.push_back(RenderComponent());
	RenderComponent& c = renderComponents[cid];
	c = RenderComponent();
	c.sprite = sprite;
	return c;
}
void RenderSystem::RemoveComponent(EntityId id) {
	int cid = entityToComponentMap[id - 1];
	entityToComponentMap[id - 1] = 0;
	renderComponents.erase(renderComponents.begin() + cid);
	componentToEntityMap.erase(componentToEntityMap.begin() + cid);

	int size = componentToEntityMap.size();
	for (int i = cid; i < size; ++i) {
		EntityId eid = componentToEntityMap[i];
		entityToComponentMap[eid - 1] -= 1;
	}

}

