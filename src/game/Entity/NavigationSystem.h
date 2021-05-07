#pragma once

#include "Entity.h"
#include "Component.h"
#include "Archetype.h"/

class NavigationSystem {
public:
	void UpdateNavigation(NavigationArchetype& archetype);
	void MoveEntities(MovementArchetype& archetype);
};