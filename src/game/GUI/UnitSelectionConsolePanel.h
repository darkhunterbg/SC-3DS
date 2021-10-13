#pragma once

#include "../Entity/Entity.h"

#include "../MathLib.h"
#include "../Color.h"

#include <vector>

class EntityManager;
struct UnitComponent;
struct UnitDataComponent;
struct UnitHealthComponent;

class UnitSelectionConsolePanel {

public:
	void UpdateSelection();

	void Draw();
	Rectangle PanelDst;
private:

	void DrawMultiSelection(Rectangle dst);
	
	void DrawUnitName(Rectangle space, EntityId id, const UnitComponent& unit);
	void DrawSupplyInfo(Rectangle space, EntityId id, const UnitComponent& unit);
	void DrawProductionDetails(Rectangle space, EntityId id, const UnitDataComponent& data);
	void DrawUnitDetail(Rectangle space, EntityId id, const UnitComponent& unit);

	void DrawUnitInfo(Rectangle space, EntityId id, const UnitComponent& unit, const UnitHealthComponent& health);

	void GetUnitWireframeColors(EntityId id, const UnitHealthComponent& health, Color outColors[4]);
};