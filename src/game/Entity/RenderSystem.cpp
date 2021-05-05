#include "RenderSystem.h"
#include "../Platform.h"
#include "../Profiler.h"
#include <algorithm>

#include <cstring>


void RenderSystem::Draw(const Camera& camera) {
	Rectangle camRect = camera.GetRectangle();

	constexpr Color4 shadowColor = Color4(0.0f, 0.0f, 0.0f, 0.5f);

	float camMul = 1.0f / camera.Scale;

	render.clear();

	for (RenderComponent& cmp : RenderComponents.GetComponents())
	{
		// TODO: size
		if (!camRect.Contains(cmp._dst))
			continue;

		Vector2Int dst = cmp._dst;
		dst -= camRect.position;
		dst /= camera.Scale;

		Vector2Int shadowDst = cmp._shadowDst;
		shadowDst -= camRect.position;
		shadowDst /= camera.Scale;

		int order = cmp.depth * 10'000'000;
		order += dst.y * 1000 + dst.x * 3;

		Vector2 flip = { cmp.hFlip ? -1.0f : 1.0f,1.0f };

		BatchDrawCommand cmd;
		cmd.order = order;
		cmd.image = cmp.shadowSprite;
		cmd.position = shadowDst;
		cmd.scale = flip * camMul;
		cmd.color = { shadowColor, 1 };
		render.push_back(cmd);

		cmd.order++;
		cmd.image = cmp.sprite;
		cmd.position = dst;
		cmd.color = { Color4(Colors::Black),0 };
		render.push_back(cmd);

		cmd.order++;
		cmd.image = cmp.colorSprite;
		cmd.color = { Color4(cmp.unitColor), 0.66f };
		render.push_back(cmd);
	}

	std::sort(render.begin(), render.end(), RenderSort);


	Platform::BatchDraw({ render.data(),render.size() });
}

bool RenderSystem::RenderSort(const BatchDrawCommand& a, const BatchDrawCommand& b) {
	return a.order < b.order;
}

void RenderSystem::UpdateEntities(const Span<Entity> entities) {

	for (const Entity& entity : entities) {
		if (!entity.HasComponent<RenderComponent>())
			continue;

		RenderComponent& c = RenderComponents.GetComponent(entity.id);
		c._dst = entity.position + c.offset;
		c._shadowDst = entity.position + c.shadowOffset;
	}

}


