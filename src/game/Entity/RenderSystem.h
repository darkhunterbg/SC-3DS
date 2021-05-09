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

class EntityManager;
class Camera;

class RenderSystem {

private:
	std::vector< BatchDrawCommand> render;

	RenderArchetype renderArchetype;
	RenderUpdatePositionArchetype renderUpdatePosArchetype;


	void CameraCull(const Rectangle16& rect, EntityManager& em);

	static bool RenderSort(const BatchDrawCommand& a, const BatchDrawCommand& b);
public:
	void Draw(const Camera& camera, EntityManager& em);
	void UpdatePositions(EntityManager& em);

};