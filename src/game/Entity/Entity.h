#pragma once

#include "../MathLib.h"
#include <bitset>

typedef unsigned int EntityId;

struct Entity {

	EntityId id = 0;
	Vector2Int position = { 0,0 };
	int orientation = 0;
	std::bitset<32> components;

	bool changed = false;

	template <class TComponent>
	inline bool HasComponent() const {
		return components.test(TComponent::ComponentId);
	}
	template <class TComponent>
	inline void SetHasComponent(bool hasComponent) {
		components.set(TComponent::ComponentId, hasComponent);
	}

	static constexpr const int MaxEntities = 5000;
	static constexpr const EntityId None = 0;
};

inline constexpr const int EntityIdToIndex(EntityId id) { return id - 1; }

