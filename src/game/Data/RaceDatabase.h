#pragma once

#include "RaceDef.h"
#include "UnitDef.h"

class RaceDatabase {
private:
	RaceDatabase() = delete;
	~RaceDatabase() = delete;
public:
	static RaceDef Terran, Zerg, Protoss;

	static void Init();
};