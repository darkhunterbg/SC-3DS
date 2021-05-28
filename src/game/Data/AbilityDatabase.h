#pragma once


#include "AbilityDef.h"
#include <vector>

class AbilityDatabase {
private:
	AbilityDatabase() = delete;
	~AbilityDatabase() = delete;

public:
	static AbilityDef Move, Stop, Attack, Patrol, HoldPosition, BuildUnit;

	static std::vector<AbilityDef*> Abilities;

	static void Init();
};