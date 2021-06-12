#pragma once

#include "../Entity/Entity.h"
#include "../MathLib.h"
#include "../Assets.h"

#include "GameViewContext.h"

#include <array>

class EntityManager;
struct AbilityDef;

class UnitCommandsPanel {
private:
	struct UnitCommand {
		const AbilityDef* ability = nullptr;
		const SpriteFrame* commandIcon = nullptr;
		const UnitDef* abilityProduce = nullptr;

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

	void DrawCommands(GameViewContext& context);

	void UpdateCommands(GameViewContext& context);

	void OnCommandPressed(GameViewContext& context, const UnitCommand& cmd);

	int pressedCommand = -1;
	int hover = -1;
public:
	Rectangle PanelDst;


	void Draw(GameViewContext& context);
	void UpdateInput(GameViewContext& context);
};