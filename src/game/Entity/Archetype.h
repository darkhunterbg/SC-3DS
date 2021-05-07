#pragma once

#include "Entity.h"
#include "Component.h"
#include <vector>

template <class T>
using ArchetypeCollection = std::vector<T>;

struct RenderArchetype {
	ArchetypeCollection<RenderDestinationComponent> pos;
	ArchetypeCollection<RenderComponent> ren;
};
struct RenderUpdatePositionArchetype {
	ArchetypeCollection<RenderDestinationComponent*> outPos;
	ArchetypeCollection<Vector2Int> worldPos;
	ArchetypeCollection<RenderOffsetComponent> offset;
};