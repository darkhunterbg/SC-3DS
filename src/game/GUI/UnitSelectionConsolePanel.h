#pragma once

#include "../Entity/Entity.h"

#include "../MathLib.h"
#include "../Color.h"
#include "GameViewContext.h"

#include <vector>

class EntityManager;
struct UnitComponent;
struct UnitDataComponent;
struct UnitHealthComponent;

class UnitSelectionConsolePanel {

public:
	void UpdateSelection(GameViewContext& context);

	void Draw(GameViewContext& context);
	Rectangle PanelDst;
private:

	void DrawMultiSelection(Rectangle dst, GameViewContext& context);
	
	void DrawUnitName(Rectangle space, EntityId id, const UnitComponent& unit, GameViewContext& context);
	void DrawSupplyInfo(Rectangle space, EntityId id, const UnitComponent& unit, GameViewContext& context);
	void DrawProductionDetails(Rectangle space, EntityId id, const UnitDataComponent& data, GameViewContext& context);
	void DrawUnitDetail(Rectangle space, EntityId id, const UnitComponent& unit, GameViewContext& context);

	void DrawUnitInfo(Rectangle space, EntityId id, const UnitComponent& unit, const UnitHealthComponent& health);

	void GetUnitWireframeColors(EntityId id, const UnitHealthComponent& health, Color outColors[4]);
};