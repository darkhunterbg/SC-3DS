#pragma once

#include "../MathLib.h"
#include "../Entity/Common.h"
#include "../Assets.h"

#include <string> 

struct AbilityDef {

	std::string Name;
	int IconId;

	 SpriteFrame Sprite;

	AbilityDef() {}
	AbilityDef(const AbilityDef&) = delete;
	AbilityDef& operator=(const AbilityDef&) = delete;
};