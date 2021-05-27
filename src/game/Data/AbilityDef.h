#pragma once

#include "../MathLib.h"
#include "../Entity/Common.h"
#include "../Assets.h"

#include <string> 

struct AbilityDef {

	int AbilityId = 0;

	std::string Name;
	int IconId;

	SpriteFrame Sprite;

	struct {
		bool HasTargetSelection = false;
		UnitAIState EntitySelectedAction = UnitAIState::Nothing;
		UnitAIState PositionSelectedAction = UnitAIState::Nothing;

		inline void SetAllStates(UnitAIState s) {
			EntitySelectedAction = PositionSelectedAction = s;
		}
		inline bool IsState(UnitAIState s) const {
			return EntitySelectedAction == s || PositionSelectedAction == s;
		}
	} TargetingData;

	AbilityDef() {}
	AbilityDef(const AbilityDef&) = delete;
	AbilityDef& operator=(const AbilityDef&) = delete;
};