#pragma once

#include "UnitDef.h"
#include <vector>


class UnitDatabase {

private:
	UnitDatabase() = delete;
	~UnitDatabase() = delete;
public:
	static UnitDef Marine, SCV;
	static UnitDef CommandCenter;

	static std::vector<UnitDef*> Units;

	static void Init();
	static void LoadAllUnitResources();
	static void LoadUnitResources(UnitDef& def);
};