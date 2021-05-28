#pragma once

#include <cstdint>

typedef uint8_t PlayerId;

enum class UnitState : uint8_t {
	Idle = 0,
	Turning = 1,
	Movement = 2,
	Attacking = 3,
	Death = 4,
};

enum class UnitAIState :uint8_t {
	Idle = 0,
	AttackTarget = 1,
	GoToPosition = 2,
	GoToAttack = 3,
	HoldPosition = 4,
	Patrol = 5,
	Follow = 6,

	Nothing = 0xFF,
};

enum class TimerExpiredAction : uint8_t {
	None = 0,
	WeaponAttack = 1,
	UnitDeathAfterEffect = 2,
	DeleteEntity = 3,

};

enum UnitChatType {
	None = 0,
	Select = 1,
	Command = 2,
	Ready = 3,
	Advisor = 4,
};

enum AdvisorErrorMessageType {
	NotEnoughMinerals = 0,
	NotEnoughGas = 1,
	NotEnoughSupply = 2,
	LandingInterrupted = 3,
	InvalidLandingZone = 4,
	NotEnoughEnergy = 6,
};


static constexpr const int TimerExpiredActionTypeCount = 4;