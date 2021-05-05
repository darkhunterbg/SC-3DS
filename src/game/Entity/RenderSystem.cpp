#include "RenderSystem.h"
#include "../Platform.h"
#include "../Profiler.h"
#include <algorithm>

#include <cstring>


void RenderSystem::Draw(const Camera& camera) {
	Rectangle camRect = camera.GetRectangle();

	SectionProfiler p("GenerateDraws");

	constexpr Color4 shadowColor = Color4(0.0f, 0.0f, 0.0f, 0.5f);

	float camMul = 1.0f / camera.Scale;

	render.clear();

	for (RenderComponent& cmp : RenderComponents.GetComponents())
	{
		if (!camRect.Intersects(cmp._dst))
			continue;

		Rectangle dst = cmp._dst;
		dst.position -= camRect.position;
		dst.position /= camera.Scale;

		Rectangle shadowDst = cmp._shadowDst;
		shadowDst.position -= camRect.position;
		shadowDst.position /= camera.Scale;

		int order = cmp.depth * 10'000'000;
		order += dst.position.y * 1000 + dst.position.x * 3;

		Vector2 flip = { cmp.hFlip ? -1.0f : 1.0f,1.0f };

		BatchDrawCommand cmd;
		cmd.order = order;
		cmd.image = cmp.shadowSprite.image;
		cmd.position = shadowDst.position;
		cmd.scale = flip * camMul;
		cmd.color = { shadowColor, 1 };
		render.push_back(cmd);

		cmd.order++;
		cmd.image = cmp.sprite.image;
		cmd.position = dst.position;
		cmd.color = { Color4(Colors::Black),0 };
		render.push_back(cmd);

		cmd.order++;
		cmd.image = cmp.colorSprite.image;
		cmd.color = { Color4(cmp.unitColor), 0.66f };
		render.push_back(cmd);
	}

	p.Submit();

	SectionProfiler p2("SortDraws");

	std::sort(render.begin(), render.end(), RenderSort);

	p2.Submit();


	SectionProfiler p3("ExecuteDraws");

	Platform::BatchDraw({ render.data(),render.size() });


	p3.Submit();
}

bool RenderSystem::RenderSort(const BatchDrawCommand& a, const BatchDrawCommand& b) {
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


