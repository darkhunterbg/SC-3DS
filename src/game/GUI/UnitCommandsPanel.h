#pragma once

#include "../Entity/Entity.h"
#include "../MathLib.h"
#include "../Assets.h"

#include "GameHUDContext.h"

#include <array>

class EntityManager;
class AbilityDef;

class UnitCommandsPanel {
private:
	struct UnitCommand {
		const AbilityDef* ability;
		bool active;
		bool enabled;
		bool pressed;
	};
	
	std::array<UnitCommand, 3 * 3> unitCommands;

	void DrawCommands(GameHUDContext& context);

	void UpdateCommands(GameHUDContext& context);

	int pressedCommand = -1;
public:
	Rectangle PanelDst;

	UnitCommandsPanel();

	void Draw(GameHUDContext& context);
	void UpdateInput(GameHUDContext& context);
};