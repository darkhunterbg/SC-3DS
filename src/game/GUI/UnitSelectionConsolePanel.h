#pragma once

#include "../Entity/Entity.h"

#include "../MathLib.h"
#include "../Color.h"
#include "GameViewContext.h"

#include <vector>

class EntityManager;
class UnitComponent;
class UnitHealthComponent;

class UnitSelectionConsolePanel {

public:
	void UpdateSelection(GameViewContext& context);

	void Draw(GameViewContext& context);
	Rectangle PanelDst;
private:

	void DrawMultiSelection(Rectangle dst, GameViewContext& context);
	void DrawUnitDetail(Rectangle space, EntityId id, const UnitComponent& unit, GameViewContext& context);
	void DrawUnitInfo(Rectangle space, EntityId id, const UnitComponent& unit, const UnitHealthComponent& health);

	void GetUnitWireframeColors(EntityId id, const UnitHealthComponent& health, Color outColors[4]);
};