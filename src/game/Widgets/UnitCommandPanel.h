#pragma once

#include "../Entity/Entity.h"
#include "../MathLib.h"
#include <vector>


class UnitCommandPanel {

	struct Command {
		const class ImageFrame* icon = nullptr;
		const char* text = nullptr;
		bool enabled = false;
		bool active = false;
		Vector2Int8 pos = {};
	};

private:
	void GenerateUnitCommands(EntityId unit);

	std::vector<Command> _commands;
public:
	void Draw(EntityId id, const struct RaceDef& skin);
};