#include "RenderSystem.h"
#include "../Platform.h"
#include "../Profiler.h"
#include <algorithm>

#include <cstring>


void RenderSystem::Draw(const Camera& camera) {
	Rectangle camRect = camera.GetRectangle();

	render.clear();

	for(RenderComponent& cmp : RenderComponents.GetComponents())
	{
		if (!camRect.Intersects(cmp._dst))
			continue;

		Rectangle dst = cmp._dst;
		dst.position -= camRect.position;
		dst.position /= camera.Scale;
		dst.size /= camera.Scale;
		int order = cmp.depth * 100000;
		order += dst.position.y * 400 + dst.position.x;
		
		render.push_back({ order, cmp.sprite, dst });
	}


	std::sort(render.begin(), render.end(), RenderSort);

	for (const auto& r : render)
	{
		Platform::Draw(r.sprite, r.dst, Colors::White);
	}
}

bool RenderSystem::RenderSort(const Render& a, const Render& b) {
	return a.order < b.order;
}

void RenderSystem::UpdateEntities(const Span<Entity> entities) {

	for (const Entity& entity : entities) {
		if (!entity.HasComponent<RenderComponent>())
			continue;

		RenderComponent& c = RenderComponents.GetComponent(entity.id);
		c._dst = c.sprite.rect;
		c._dst.position = entity.position + c.offset;
	}

}


