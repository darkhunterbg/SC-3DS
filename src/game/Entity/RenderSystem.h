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
	Vector2Int offset = { 0,0 };
};

class RenderSystem {

public:
	RenderSystem(int maxEntities);
	~RenderSystem();		

	void Draw(const Camera& camera);

	RenderComponent& NewComponent(EntityId id, const Sprite& sprite);
	RenderComponent* GetComponent(EntityId id)  {
		int cid = entityToComponentMap[id - 1];
		if (cid > -1)
			return &renderComponents[cid];

		return nullptr;
	}
	void RemoveComponent(EntityId id);

	void UpdateEntities(const Span<Entity> entity);
private:
	std::vector<RenderComponent> renderComponents;
	int maxComponents = 0;
	
	int* entityToComponentMap;
	std::vector<EntityId> componentToEntityMap;

};