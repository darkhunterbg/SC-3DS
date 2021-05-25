#pragma once

#include "../Entity/Entity.h"

#include "../MathLib.h"
#include <vector>

class EntityManager;
class UnitComponent;
class UnitHealthComponent;

class UnitSelectionConsolePanel {

public:
	void Draw(Rectangle dst, const std::vector<EntityId>& selection, EntityManager& em);
private:
	void DrawUnitDetail(Rectangle space, const UnitComponent& unit);
	void DrawUnitInfo(Rectangle space, const UnitComponent& unit, const UnitHealthComponent& health);
};