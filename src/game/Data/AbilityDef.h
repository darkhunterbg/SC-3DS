#pragma once

#include "../MathLib.h"
#include "../Assets.h"

#include <string> 

typedef uint32_t AbilityId;

#pragma pack(push,2)
struct AbilityDef {

	char Path[32];
	AbilityId Id;

	struct AbilityArt {
		char Name[32];
		uint16_t IconId;
		Vector2Int8 ButtonPosition;

		inline uint8_t GetButtonIndex() const { return ButtonPosition.x + ButtonPosition.y * 3; }
		const ImageFrame& GetIcon() const;

	} Art;

	struct AbilityData {
		//UnitAIState EntitySelectedAction;
		//UnitAIState PositionSelectedAction;
		//inline bool HasTargetSelection() const {
		//	return PositionSelectedAction != UnitAIState::Nothing;
		//}

		//	inline bool IsState(UnitAIState s) const {
		//	return EntitySelectedAction == s || PositionSelectedAction == s;
		//}
	} Data;


};
#pragma pack(pop)