#pragma once

#include <vector>
#include "../Camera.h"
#include "Entity.h"
#include "Component.h"
#include "../Assets.h"
#include "../Span.h"
#include "../Color.h"
#include "../Platform.h"
#include "Archetype.h"

class RenderSystem {

private:
	std::vector< BatchDrawCommand> render;

	static bool RenderSort(const BatchDrawCommand& a, const BatchDrawCommand& b);
public:
	void Draw(const Camera& camera, RenderArchetype& archetype);
	void SetRenderPosition(RenderUpdatePositionArchetype& archetype);

};