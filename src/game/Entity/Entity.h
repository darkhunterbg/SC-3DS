#pragma once

#include "../MathLib.h"
#include <bitset>

typedef unsigned int EntityId;

#pragma pack(push,4)
struct Entity {
	EntityId id = 0;
	Vector2Int position = { 0,0 };
	bool changed = false;
	std::bitset<32> components;

	inline int BufferPosition() const { return id - 1; }

	template <class TComponent>
	inline bool HasComponent() {
		return components.test(TComponent::ComponentId);
	}
	template <class TComponent>
	inline void SetHasComponent(bool hasComponent) {
		components.set(TComponent::ComponentId, hasComponent);
	}

	static constexpr const int MaxEntities = 5000;
};

inline constexpr const int EntityIdToIndex(EntityId id) { return id - 1; }
#pragma pack(pop)

