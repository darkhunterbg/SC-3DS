#pragma once

#include "../MathLib.h"

typedef unsigned int EntityId;

struct Entity {
	EntityId id;
	Vector2Int position;
	int renderComponent = 0;
};
