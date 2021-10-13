#pragma once

#include "../Entity/Entity.h"
#include "../MathLib.h"
#include "../Assets.h"

#include <array>

class EntityManager;
struct AbilityDef;
struct UnitDef;

class UnitCommandsPanel {
private:
	struct UnitCommand {
		const AbilityDef* ability = nullptr;
		const ImageFrame* commandIcon = nullptr;
		const UnitDef* abilityProduce = nullptr;

		bool active = false;
		bool enabled = false;
		bool pressed = false;

		bool IsVisible() const
		{
			return ability || commandIcon;
		};
		bool IsUsable() const
		{
			return IsVisible() && enabled;
		}
	};

	std::array<UnitCommand, 3 * 3> unitCommands;

	void DrawCommands();

	void UpdateCommands();

	void OnCommandPressed(const UnitCommand& cmd);

	int pressedCommand = -1;
	int hover = -1;
public:
	Rectangle PanelDst;


	void Draw();
	void UpdateInput();
};