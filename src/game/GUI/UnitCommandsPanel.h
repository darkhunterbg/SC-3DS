#pragma once

#include "../Entity/Entity.h"

#include "../MathLib.h"
#include "../Data/RaceDef.h"

#include "../Assets.h"

#include <array>

class EntityManager;

class UnitCommandsPanel {
private:
	struct UnitCommand {
		const Sprite* sprite;
		bool active;
		bool enabled;
	};
	
	std::array<UnitCommand, 3 * 3> unitCommands;

	void DrawCommands();

public:
	const RaceDef* Race = nullptr;
	Rectangle PanelDst;

	void Draw(const std::vector<EntityId>& selection, EntityManager& em);
};