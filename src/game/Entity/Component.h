#pragma once

#include <vector>
#include <array>
#include <cstring>

#include "Entity.h"
#include "../Span.h"
#include "../MathLib.h"
#include "../Color.h"
#include "../Assets.h"

////template <unsigned id>
//struct IComponent {
//	//static constexpr const unsigned ComponentId = id;
//};


template <class TComponent, unsigned MaxComponents = Entity::MaxEntities>
class ComponentCollection
{
private:
	std::vector<TComponent> components;
	std::vector<EntityId> componentToEntityMap;
	std::array<int, MaxComponents> entityToComponentMap;
public:
	ComponentCollection() {
		memset(entityToComponentMap.data(), -1, sizeof(MaxComponents) * sizeof(int));
	}

	inline Span<TComponent> GetComponents() const { return { components.data(),components.size() }; }
	inline EntityId GetEntityIdForComponent(int cid) const { return componentToEntityMap[cid]; }

	TComponent& NewComponent(EntityId id) {
		int cid = components.size();
		entityToComponentMap[Entity::ToIndex(id)] = cid;
		componentToEntityMap.push_back(id);
		components.push_back(TComponent());
		return components[cid];
	}

	TComponent& NewComponent(EntityId id, const TComponent& component) {
		int cid = components.size();
		entityToComponentMap[Entity::ToIndex(id)] = cid;
		componentToEntityMap.push_back(id);
		components.push_back(component);
		return components[cid];
	}

	void RemoveComponent(EntityId id) {
		int cid = entityToComponentMap[Entity::ToIndex(id)];
		entityToComponentMap[Entity::ToIndex(id)] = -1;
		components.erase(components.begin() + cid);
		componentToEntityMap.erase(componentToEntityMap.begin() + cid);

		int size = componentToEntityMap.size();
		for (int i = cid; i < size; ++i) {
			EntityId eid = componentToEntityMap[i];
			entityToComponentMap[Entity::ToIndex(eid)] -= 1;
		}

	}
	inline TComponent& GetComponent(EntityId id) {
		int cid = entityToComponentMap[Entity::ToIndex(id)];
		return components[cid];
	}
	inline const TComponent* GetComponent(EntityId id) const {
		int cid = entityToComponentMap[Entity::ToIndex(id)];
		return components[cid];
	}

	inline TComponent& at(int i) { return components[i]; };
	inline const TComponent& at(int i) const { return components[i]; }
	inline size_t size() const { return components.size(); }
	inline TComponent& operator[](int i) { return components[i]; }
	inline const TComponent& operator[](int i) const { return components[i]; }
};

struct EntityChangeComponent {
	bool changed;
};

struct RenderComponent {
	Color4 unitColor;
	Image sprite;
	Image shadowSprite;
	Image colorSprite;
	int depth = 0;
	bool hFlip = false;
};

struct RenderOffsetComponent  {
	Vector2Int offset;
	Vector2Int shadowOffset;
};

struct RenderDestinationComponent  {
	Vector2Int dst;
	Vector2Int shadowDst;
};


struct NavigationWorkComponent {
	bool work;
};

struct NavigationComponent {
	Vector2Int target;
	unsigned targetHeading;
};

struct MovementComponent {
	unsigned orientation;
	unsigned velocity;
	unsigned rotationSpeed;
};