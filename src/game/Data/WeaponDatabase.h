#pragma once

#include "WeaponDef.h"
#include <vector>

class WeaponDatabase {

private:
	WeaponDatabase() = delete;
	~WeaponDatabase() = delete;
public:
	static WeaponDef GaussRifle, FusionCutter;

	static std::vector<WeaponDef*> Weapons;

	static void Init();
};