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
		const AbilityDef* ability = nullptr;
		const SpriteFrame* commandIcon = nullptr;
		bool active = false;
		bool enabled = false;
		bool pressed = false;

		bool IsVisible() const {
			return ability || commandIcon;
		};
		bool IsUsable() const {
			return IsVisible() && enabled;
		}
	};
	
	std::array<UnitCommand, 3 * 3> unitCommands;

	void DrawCommands(GameHUDContext& context);

	void UpdateCommands(GameHUDContext& context);

	void OnCommandPressed(GameHUDContext& context, const UnitCommand& cmd);

	int pressedCommand = -1;
	int hover = -1;
public:
	Rectangle PanelDst;

	UnitCommandsPanel();

	void Draw(GameHUDContext& context);
	void UpdateInput(GameHUDContext& context);
};