#pragma once

#include <vector>
#include "../Camera.h"
#include "Entity.h"
#include "../Assets.h"
#include "../Span.h"


struct RenderComponent {

	int id;
	EntityId _entityId;
	Rectangle _dst;
	Sprite sprite;
};

class RenderSystem {

public:
	RenderSystem();

	void Draw(const Camera& camera);

	RenderComponent* NewComponent(EntityId entity);

	void UpdateEntities(const Span<Entity> entity);
private:
	std::vector<RenderComponent> renderComponents;

};