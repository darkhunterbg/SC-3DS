#pragma once

#include "../MathLib.h"
#include "../Assets.h"

#include "../Entity/UnitAIStateMachine.h"

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
		UnitAIStateId EntitySelectedAction;
		UnitAIStateId PositionSelectedAction;
		
	} Data;

	inline bool HasTargetSelection() const
	{
		return Data.PositionSelectedAction != UnitAIStateId::Nothing;
	}
	inline bool CanTargetEntity() const
	{
		return Data.EntitySelectedAction != UnitAIStateId::Nothing;
	}

	inline bool IsState(UnitAIStateId s) const
	{
		return Data.EntitySelectedAction == s || Data.PositionSelectedAction == s;
	}
};
#pragma pack(pop)