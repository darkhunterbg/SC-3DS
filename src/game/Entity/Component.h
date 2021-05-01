#pragma once

#include <vector>
#include <array>
#include "Entity.h"
#include "../Span.h"
#include <cstring>

template <unsigned id>
struct IComponent {
	static constexpr const unsigned ComponentId = id;
};


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

	//inline Span<TComponent> GetComponents() const { return { components.data(),components.size() }; }

	inline Span<TComponent> GetComponents() const { return { components.data(),components.size() }; }
	inline EntityId GetEntityIdForComponent(int cid) const { return componentToEntityMap[cid]; }

	TComponent& NewComponent(EntityId id) {
		int cid = components.size();
		entityToComponentMap[EntityIdToIndex(id)] = cid;
		componentToEntityMap.push_back(id);
		components.push_back(TComponent());
		return components[cid];
	}

	void RemoveComponent(EntityId id) {
		int cid = entityToComponentMap[EntityIdToIndex(id)];
		entityToComponentMap[EntityIdToIndex(id)] = -1;
		components.erase(components.begin() + cid);
		componentToEntityMap.erase(componentToEntityMap.begin() + cid);

		int size = componentToEntityMap.size();
		for (int i = cid; i < size; ++i) {
			EntityId eid = componentToEntityMap[i];
			entityToComponentMap[EntityIdToIndex(eid)] -= 1;
		}

	}
	inline TComponent& GetComponent(EntityId id) {
		int cid = entityToComponentMap[EntityIdToIndex(id)];
		return components[cid];
	}
	inline const TComponent* GetComponent(EntityId id) const {
		int cid = entityToComponentMap[EntityIdToIndex(id)];
		return components[cid];
	}
};
