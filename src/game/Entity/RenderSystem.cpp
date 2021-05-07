#include "RenderSystem.h"
#include "../Platform.h"
#include "../Profiler.h"
#include "../Job.h"

#include <algorithm>

#include <cstring>


void RenderSystem::Draw(const Camera& camera, RenderArchetype& archetype) {
	Rectangle camRect = camera.GetRectangle();

	constexpr const Color4 shadowColor = Color4(0.0f, 0.0f, 0.0f, 0.5f);

	float camMul = 1.0f / camera.Scale;

	render.clear();

	Vector2 scale[] = { {camMul,camMul},{-camMul,camMul} };

	int entitiesCount = archetype.pos.size();

	for (int i = 0; i < entitiesCount; ++i) {
		const auto& rp = archetype.pos[i];
		const auto& r = archetype.ren[i];

		Vector2Int dst = rp.dst;
		dst -= camRect.position;
		dst /= camera.Scale;

		Vector2Int shadowDst = rp.shadowDst;
		shadowDst -= camRect.position;
		shadowDst /= camera.Scale;

		int order = r.depth * 10'000'000;
		order += dst.y * 1000 + dst.x * 3;


		BatchDrawCommand cmd;
		cmd.order = order;
		cmd.image = r.shadowSprite;
		cmd.position = shadowDst;
		cmd.scale = scale[r.hFlip];
		cmd.color = { shadowColor, 1 };
		render.push_back(cmd);

		cmd.order++;
		cmd.image = r.sprite;
		cmd.position = dst;
		cmd.color = { Color4(Colors::Black),0 };
		render.push_back(cmd);

		cmd.order++;
		cmd.image = r.colorSprite;
		cmd.color = { Color4(r.unitColor), 0.66f };
		render.push_back(cmd);
	}

	std::sort(render.begin(), render.end(), RenderSort);


	Platform::BatchDraw({ render.data(),render.size() });
}

bool RenderSystem::RenderSort(const BatchDrawCommand& a, const BatchDrawCommand& b) {
	return a.order < b.order;
}


void RenderSystem::SetRenderPosition(RenderUpdatePositionArchetype& a) {
	int size = a.outPos.size();
	static RenderUpdatePositionArchetype& archetype = a;

	JobSystem::RunJob(size, JobSystem::DefaultJobSize, [](int start, int end) {

		for (int i = start; i < end; ++i) {
			RenderDestinationComponent* p = archetype.outPos[i];
			p->dst = archetype.worldPos[i] + archetype.offset[i].offset;
			p->shadowDst = archetype.worldPos[i] + archetype.offset[i].shadowOffset;
			archetype.outBB[i]->position = p->dst;
		}
		});
}

