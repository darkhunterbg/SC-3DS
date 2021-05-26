#include "GameHUDContext.h"

#include "../Entity/EntityUtil.h"
#include "../Debug.h"

void GameHUDContext::ActivateCurrentAbility()
{
	if (currentAbility == nullptr)
		EXCEPTION("Tried to active null AbilityDef!");

	auto action = currentAbility->TargetingData.EntitySelectedAction;
	if (action == UnitAIState::Nothing)
		EXCEPTION("Tried to active ability %s with Nothing action for entity!", currentAbility->Name.data());

	for (EntityId id : selectedEntities) {
		UnitEntityUtil::SetAIState(id, action);
	}
}

void GameHUDContext::ActivateCurrentAbility(EntityId target)
{
	if (currentAbility == nullptr)
		EXCEPTION("Tried to active null AbilityDef!");

	auto action = currentAbility->TargetingData.EntitySelectedAction;
	if (action == UnitAIState::Nothing)
		EXCEPTION("Tried to active ability %s with Nothing action for entity!", currentAbility->Name.data());

	for (EntityId id : selectedEntities) {
		UnitEntityUtil::SetAIState(id, action, target);
	}

	CancelTargetSelection();
}

void GameHUDContext::ActivateCurrentAbility(Vector2Int16 position)
{
	if (currentAbility == nullptr)
		EXCEPTION("Tried to active null AbilityDef!");

	auto action = currentAbility->TargetingData.PositionSelectedAction;
	if (action == UnitAIState::Nothing)
		EXCEPTION("Tried to active ability %s with Nothing action for position!", currentAbility->Name.data());

	for (EntityId id : selectedEntities) {
		UnitEntityUtil::SetAIState(id, currentAbility->TargetingData.EntitySelectedAction, position);
	}

	CancelTargetSelection();
}

void GameHUDContext::SelectAbilityTarget(const AbilityDef& def)
{
	currentAbility = &def;
	IsTargetSelectionMode = true;

	if (!def.TargetingData.HasTargetSelection)
	{
		ActivateCurrentAbility();
		CancelTargetSelection();
	}
}

void GameHUDContext::CancelTargetSelection()
{
	currentAbility = nullptr;
	IsTargetSelectionMode = false;
}
