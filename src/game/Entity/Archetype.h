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
	ArchetypeCollection<Rectangle*> outBB;
	ArchetypeCollection<Vector2Int> worldPos;
	ArchetypeCollection<RenderOffsetComponent> offset;
};

struct NavigationArchetype {
	ArchetypeCollection<NavigationWorkComponent*> work;
	ArchetypeCollection<NavigationComponent> navigation;
	ArchetypeCollection<Vector2Int*> position;
	ArchetypeCollection<EntityChangeComponent*> changed;
	ArchetypeCollection<MovementComponent*> movement;

	inline void clear() {
		work.clear();
		navigation.clear();
		position.clear();
		changed.clear();
		movement.clear();
	}

	inline size_t size() const {
		return work.size();
	}
};