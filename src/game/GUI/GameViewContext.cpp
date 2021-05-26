#include "GameViewContext.h"

#include "../Entity/EntityManager.h" 
#include "../Entity/EntityUtil.h"
#include "../Debug.h"

#include <algorithm>

void GameViewContext::ActivateAbility(const AbilityDef* ability)
{
	if (selection.size() == 0)
		return;

	if (ability == nullptr) {
		EXCEPTION("Tried to active null AbilityDef!");
		return;
	}

	auto action = ability->TargetingData.EntitySelectedAction;
	if (action == UnitAIState::Nothing) {
		EXCEPTION("Tried to active ability %s with Nothing action for entity!", ability->Name.data());
		return;
	}

	for (EntityId id : selection) {
		UnitEntityUtil::SetAIState(id, action);
	}
}

void GameViewContext::ActivateAbility(const AbilityDef* ability, EntityId target)
{
	if (selection.size() == 0)
		return;

	if (ability == nullptr) {
		EXCEPTION("Tried to active null AbilityDef!");
		return;
	}

	auto action = ability->TargetingData.EntitySelectedAction;
	if (action == UnitAIState::Nothing) {
		EXCEPTION("Tried to active ability %s with Nothing action for entity!", ability->Name.data());
		return;
	}

	for (EntityId id : selection) {
		UnitEntityUtil::SetAIState(id, action, target);
	}

	PlayUnitSelectedAudio(UnitChatType::Command);
	//NewActionMarker(position);
}

void GameViewContext::ActivateAbility(const AbilityDef* ability, Vector2Int16 position)
{
	if (selection.size() == 0)
		return;

	if (ability == nullptr) {
		EXCEPTION("Tried to active null AbilityDef!");
		return;
	}

	auto action = ability->TargetingData.PositionSelectedAction;
	if (action == UnitAIState::Nothing) {
		EXCEPTION("Tried to active ability %s with Nothing action for position!", ability->Name.data());
		return;
	}

	for (EntityId id : selection) {
		UnitEntityUtil::SetAIState(id, action, position);
	}

	PlayUnitSelectedAudio(UnitChatType::Command);
	NewActionMarker(position);
}

void GameViewContext::ActivateCurrentAbility()
{
	ActivateAbility(currentAbility);
}

void GameViewContext::ActivateCurrentAbility(EntityId target)
{
	ActivateAbility(currentAbility, target);
	CancelTargetSelection();
}

void GameViewContext::ActivateCurrentAbility(Vector2Int16 position)
{
	ActivateAbility(currentAbility, position);
	CancelTargetSelection();
}

void GameViewContext::SelectAbilityTarget(const AbilityDef& def)
{
	currentAbility = &def;
	IsTargetSelectionMode = true;

	if (!def.TargetingData.HasTargetSelection)
	{
		ActivateCurrentAbility();
		CancelTargetSelection();
	}
}

void GameViewContext::CancelTargetSelection()
{
	currentAbility = nullptr;
	IsTargetSelectionMode = false;
}

void GameViewContext::NewActionMarker(Vector2Int16 pos)
{
	GUIActionMarker marker;
	marker.pos = pos;
	marker.timer = MarkerTimer;

	markers.clear();
	markers.push_back(marker);
}

void GameViewContext::SelectUnitsInRegion(const Rectangle16 region)
{
	static std::vector<EntityId> castResults;

	auto& em = GetEntityManager();

	castResults.clear();
	em.RectCast(region, castResults);

	for (int i = 0; i < castResults.size(); ++i) {
		if (!IsVisibleUnit(castResults[i])) {
			castResults.erase(castResults.begin() + i);
			--i;
			continue;
		}
	}

	if (castResults.size() > 0)
	{
		bool allySelection = false;

		for (EntityId id : castResults) {
			if (UnitEntityUtil::IsAlly(player, id)) {
				allySelection = true;
				break;
			}
		}

		static std::vector<EntityId> finalSelection;

		if (allySelection) {
			finalSelection.clear();
			for (EntityId id : castResults) {
				if (UnitEntityUtil::IsAlly(player, id)) {
					finalSelection.push_back(id);
				}
			}

			std::sort(finalSelection.begin(), finalSelection.end());
			selection.clear();
			selection.AddSortedEntities(finalSelection);
		}
		else {
			selection.clear();
			selection.AddEntity(castResults[0]);
		}

		if (HasSelectionControl())
			PlayUnitSelectedAudio(UnitChatType::Select);

		selectionColor = GetAlliedUnitColor(selection[0]);
	}
}

void GameViewContext::SelectUnitAtPosition(Vector2Int16 position)
{
	EntityId id = GetUnitAtPosition(position);

	if (id != Entity::None) {
		selection.clear();
		selection.AddEntity(id);

		if (HasSelectionControl())
			PlayUnitSelectedAudio(UnitChatType::Select);

		selectionColor = GetAlliedUnitColor(selection[0]);
	}
}

EntityId GameViewContext::GetUnitAtPosition(Vector2Int16 position)
{
	auto& em = GetEntityManager();

	EntityId id = GetEntityManager().PointCast(position);

	if (!IsVisibleUnit(id))
		id = Entity::None;

	return id;
}

bool GameViewContext::IsVisibleUnit(EntityId id)
{
	auto& em = GetEntityManager();

	if (id != Entity::None &&
		(!em.UnitArchetype.Archetype.HasEntity(id) ||
			em.UnitArchetype.HiddenArchetype.Archetype.HasEntity(id))) {

		return false;
	}

	return true;
}

EntityId GameViewContext::GetPriorityUnitSelected() const
{
	if (selection.size() > 0)
		return selection[0];
	return Entity::None;
}

bool GameViewContext::HasSelectionControl() const
{
	if (!selection.size())
		return false;

	return UnitEntityUtil::IsAlly(player, selection[0]);
}

void GameViewContext::PlayUnitSelectedAudio(UnitChatType type)
{
	EntityId id = GetPriorityUnitSelected();

	if (id == Entity::None) {
		EXCEPTION("Tried to play audio for entity id none in group!");
	}

	GetEntityManager().GetSoundSystem().PlayUnitChat(id, type);
}

Color GameViewContext::GetAlliedUnitColor(EntityId id) {
	if (UnitEntityUtil::IsAlly(player, id))
		return Colors::UIDarkGreen;

	if (UnitEntityUtil::IsEnemy(player, id))
		return Colors::UIDarkRed;

	return Colors::UIDarkYellow;
}