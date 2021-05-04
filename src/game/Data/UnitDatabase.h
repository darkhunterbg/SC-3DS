#pragma once

#include "UnitDef.h"
#include <vector>

class UnitDatabase {

private:
	UnitDatabase() = delete;
	~UnitDatabase() = delete;
public:
	static UnitDef Marine, SCV;

	static std::vector<const UnitDef*> Units;

	static void Init();
	static void LoadAllUnitResources();
};