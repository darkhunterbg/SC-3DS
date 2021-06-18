#pragma once

#include "../MathLib.h"
#include "../Entity/Common.h"
#include "../Assets.h"

#include <string> 

typedef uint32_t AbilityId;

struct AbilityDef {

	char Path[32];
	AbilityId Id;

	struct Art {
		char Name[32];
		uint16_t IconId;

		const ImageFrame& GetIcon() const;

	} Art;

	//struct {
	//	bool HasTargetSelection = false;
	//	UnitAIState EntitySelectedAction = UnitAIState::Nothing;
	//	UnitAIState PositionSelectedAction = UnitAIState::Nothing;

	//	inline void SetAllStates(UnitAIState s) {
	//		EntitySelectedAction = PositionSelectedAction = s;
	//	}
	//	inline bool IsState(UnitAIState s) const {
	//		return EntitySelectedAction == s || PositionSelectedAction == s;
	//	}
	//} TargetingData;

};