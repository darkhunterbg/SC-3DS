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
	ArchetypeCollection<Rectangle16*> outBB;
	ArchetypeCollection<Vector2Int16> worldPos;
	ArchetypeCollection<RenderOffsetComponent> offset;
};

struct MovementArchetype {
	ArchetypeCollection<NavigationWorkComponent*> work;
	ArchetypeCollection<MovementComponent*> movement;
	ArchetypeCollection<Vector2Int*> position;
	ArchetypeCollection<NavigationComponent> navigation;
	ArchetypeCollection<EntityChangeComponent*> changed;
	ArchetypeCollection<UnitComponent> unit;
	ArchetypeCollection<AnimationEnableComponent*> animEnabled;
	ArchetypeCollection<AnimationTrackerComponent*>animTracker;
	ArchetypeCollection<AnimationComponent*> anim;

	inline void clear() {
		work.clear();
		movement.clear();
		position.clear();
		navigation.clear();
		changed.clear();
		unit.clear();
		animEnabled.clear();
		animTracker.clear();
		anim.clear();
	}

	inline size_t size() const {
		return work.size();
	}
};

struct NavigationArchetype {
	ArchetypeCollection<NavigationComponent*> navigation;
	ArchetypeCollection<Vector2Int> position;
	ArchetypeCollection<MovementComponent> movement;

	inline void clear() {
		navigation.clear();
		position.clear();
		movement.clear();
	}

	inline size_t size() const {
		return navigation.size();
	}
};

