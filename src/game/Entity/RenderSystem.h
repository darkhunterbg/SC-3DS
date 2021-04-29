#pragma once

#include <vector>
#include "../Camera.h"
#include "Entity.h"
#include "../Assets.h"
#include "../Span.h"



struct RenderComponent {
	static constexpr const int ComponentId = 0;

	Rectangle _dst;
	Sprite sprite;
};

class RenderSystem {

public:
	RenderSystem(int maxEntities);
	~RenderSystem();

	void Draw(const Camera& camera);

	RenderComponent& NewComponent(EntityId id, const Sprite& sprite);
	void RemoveComponent(EntityId id);

	void UpdateEntities(const Span<Entity> entity);
private:
	std::vector<RenderComponent> renderComponents;
	int maxComponents = 0;
	
	int* entityToComponentMap;
	std::vector<EntityId> componentToEntityMap;
};