#include "AbilityDatabase.h"
#include "Generated.h"

AbilityDef AbilityDatabase::Move,
AbilityDatabase::Stop,
AbilityDatabase::Attack,
AbilityDatabase::Patrol,
AbilityDatabase::HoldPosition,
AbilityDatabase::BuildUnit,
AbilityDatabase::Gather;

std::vector<AbilityDef*> AbilityDatabase::Abilities = {
	&AbilityDatabase::Move,
	&AbilityDatabase::Stop,
	&AbilityDatabase::Attack,
	&AbilityDatabase::Patrol,
	&AbilityDatabase::HoldPosition,
	&AbilityDatabase::BuildUnit,
	&AbilityDatabase::Gather
};

static int Id = 0;

void AbilityDatabase::Init()
{
	Move.Name = "Move";
	Stop.Name = "Stop";
	Attack.Name = "Attack";
	Patrol.Name = "Patrol";
	HoldPosition.Name = "HoldPosition";
	BuildUnit.Name = "Build";
	Gather.Name = "Gather";

	Move.IconId = 228;
	Stop.IconId = 229;
	Attack.IconId = 230;
	Patrol.IconId = 254;
	HoldPosition.IconId = 255;
	Gather.IconId = 231;

	Move.TargetingData.HasTargetSelection = true;
	Move.TargetingData.PositionSelectedAction = UnitAIState::GoToPosition;
	Move.TargetingData.EntitySelectedAction = UnitAIState::Follow;

	Stop.TargetingData.SetAllStates(UnitAIState::Idle);

	Attack.TargetingData.HasTargetSelection = true;
	Attack.TargetingData.PositionSelectedAction = UnitAIState::GoToAttack;
	Attack.TargetingData.EntitySelectedAction = UnitAIState::AttackTarget;

	HoldPosition.TargetingData.SetAllStates(UnitAIState::HoldPosition);

	Patrol.TargetingData.HasTargetSelection = true;
	Patrol.TargetingData.SetAllStates(UnitAIState::Patrol);

	Gather.TargetingData.HasTargetSelection = true;
	Gather.TargetingData.EntitySelectedAction = (UnitAIState::GatherResource);
	Gather.TargetingData.PositionSelectedAction = (UnitAIState::GoToPosition);

	const auto& sa = *SpriteDatabase::Load_unit_cmdbtns_cmdicons();

	for (auto& def : Abilities) {
		def->AbilityId = Id++;
		def->Sprite = sa.GetFrame(def->IconId);
	}
}
