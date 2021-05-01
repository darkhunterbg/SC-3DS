#include "RenderSystem.h"
#include "../Platform.h"
#include "../Profiler.h"
#include <cstring>


void RenderSystem::Draw(const Camera& camera) {
	Rectangle camRect = camera.GetRectangle();

	for(RenderComponent& cmp : RenderComponents.GetComponents())
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

	for (const Entity& entity : entities) {
		RenderComponent& c = RenderComponents.GetComponent(entity.id);
		c._dst = c.sprite.rect;
		c._dst.position = entity.position + c.offset;
	}

}


