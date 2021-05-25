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
};


enum class TimerExpiredAction : uint8_t {
	None = 0,
	WeaponAttack = 1,
	UnitDeathAfterEffect = 2,
	DeleteEntity = 3,

};



static constexpr const int TimerExpiredActionTypeCount = 4;