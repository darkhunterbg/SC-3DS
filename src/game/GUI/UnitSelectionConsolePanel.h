#pragma once

#include "../Entity/Entity.h"

#include "../MathLib.h"
#include "../Color.h"
#include "GameHUDContext.h"

#include <vector>

class EntityManager;
class UnitComponent;
class UnitHealthComponent;

class UnitSelectionConsolePanel {

public:
	void UpdateSelection(GameHUDContext& context);

	void Draw(GameHUDContext& context);
	Rectangle PanelDst;
private:

	void DrawMultiSelection(Rectangle dst, GameHUDContext& context);
	void DrawUnitDetail(Rectangle space, EntityId id, const UnitComponent& unit, GameHUDContext& context);
	void DrawUnitInfo(Rectangle space, EntityId id, const UnitComponent& unit, const UnitHealthComponent& health);

	void GetUnitWireframeColors(EntityId id, const UnitHealthComponent& health, Color outColors[4]);
};