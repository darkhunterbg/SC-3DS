#include "RenderSystem.h"
#include "../Platform.h"

RenderSystem::RenderSystem() {
	renderComponents.reserve(5000);
}

void RenderSystem::Draw(const Camera& camera) {
	Rectangle camRect = camera.GetRectangle();

	for (const RenderComponent& cmp : renderComponents) {
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
	for (const Entity& entity : entities) {
		RenderComponent c = renderComponents[entity.renderComponent];
		c._dst = c.sprite.rect;
		c._dst.position = entity.position;

		renderComponents[entity.renderComponent] = c;
	}
}

RenderComponent* RenderSystem::NewComponent(EntityId id) {

	RenderComponent c;
	c.id = renderComponents.size();
	c._entityId = id;
	renderComponents.push_back(c);
	return &renderComponents[c.id];
}

