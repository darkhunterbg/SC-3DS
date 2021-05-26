#pragma once

#include "../Entity/Entity.h"

#include "../MathLib.h"
#include "../Data/RaceDef.h"
#include "../Color.h"

#include <vector>

class EntityManager;
class UnitComponent;
class UnitHealthComponent;

class UnitSelectionConsolePanel {

public:
	void UpdateSelection( std::vector<EntityId>& selection);

	void Draw(const std::vector<EntityId>& selection, EntityManager& em);
	const RaceDef* Race = nullptr;
	Rectangle PanelDst;
private:

	void DrawMultiSelection(Rectangle dst, const std::vector<EntityId>& selection, EntityManager& em);
	void DrawUnitDetail(Rectangle space, EntityId id, const UnitComponent& unit, EntityManager& em);
	void DrawUnitInfo(Rectangle space, EntityId id, const UnitComponent& unit, const UnitHealthComponent& health);

	void GetUnitWireframeColors(EntityId id, const UnitHealthComponent& health, Color outColors[4]);
};