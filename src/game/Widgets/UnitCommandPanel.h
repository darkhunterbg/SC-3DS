#pragma once

#include "../Entity/Entity.h"
#include "../MathLib.h"
#include <vector>


class UnitCommandPanel {
public:
	struct Command {
		const class ImageFrame* icon = nullptr;
		const char* text = nullptr;
		bool enabled = false;
		bool active = false;
		Vector2Int8 pos = {};

		const struct AbilityDef* ability = nullptr;

		inline bool IsAbility() const { return ability != nullptr; }
	};

	int _commandActivateDelay = 0;
	Command _activatedCmd;

	const struct SoundSetDef* _buttonSound = nullptr;
private:
	std::vector<Command> _commands;
	const Command* DrawCommandButtonsAndSelect(const struct RaceDef& skin);
public:
	UnitCommandPanel();

	const Command* DrawUnitCommandsAndSelect(EntityId id, const struct RaceDef& skin);
	const Command* DrawAbilityCommandsAndSelect(const struct AbilityDef* ability, const struct RaceDef& skin);
};