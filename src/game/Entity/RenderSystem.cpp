#include "RenderSystem.h"
#include "../Platform.h"
#include "../Profiler.h"
#include <algorithm>

#include <cstring>


void RenderSystem::Draw(const Camera& camera) {
	Rectangle camRect = camera.GetRectangle();

	render.clear();

	// Shadow rendering:
	// Get all object to render in order
	// Render shadow with additive blending, then object

	for (RenderComponent& cmp : RenderComponents.GetComponents())
	{
		if (!camRect.Intersects(cmp._dst))
			continue;

		Rectangle dst = cmp._dst;
		dst.position -= camRect.position;
		dst.position /= camera.Scale;
		dst.size /= camera.Scale;


		Rectangle shadowDst = cmp._shadowDst;
		shadowDst.position -= camRect.position;
		shadowDst.position /= camera.Scale;
		shadowDst.size /= camera.Scale;

		int order = cmp.depth * 10'000'000;
		order += dst.position.y * 1000 + dst.position.x;


		render.push_back({ order, cmp.sprite, dst, cmp.hFlip, cmp.shadowSprite, shadowDst });
	}


	std::sort(render.begin(), render.end(), RenderSort);

	Color c = Colors::Black;
	c.a = 0.5f;

	for (const auto& r : render)
	{
		if (r.shadowDst.size.LengthSquared() > 0)
			Platform::Draw(r.shadowSprite, r.shadowDst, c, r.hFlip);

		Platform::Draw(r.sprite, r.dst, Colors::Black, r.hFlip);
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

		c._shadowDst = c.shadowSprite.rect;
		c._shadowDst.position = entity.position + c.shadowOffset;
	}

}


