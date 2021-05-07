#pragma once

#include "../MathLib.h"
#include <bitset>

typedef unsigned int EntityId;

class Entity {
private:
	Entity() = delete;
	~Entity() = delete;
public:
	static constexpr const int MaxEntities = 10'000;
	static constexpr const EntityId None = 0;
	static inline constexpr const int ToIndex(EntityId id) { return id - 1; }
};



