#include "GameViewContext.h"

#include "../Entity/EntityManager.h" 
#include "../Entity/EntityUtil.h"
#include "../Debug.h"
#include "../Data/GameDatabase.h"

#include <algorithm>

void GameViewContext::ActivateAbility(const AbilityDef* ability)
{
	if (selection.size() == 0)
		return;

	if (ability == nullptr) {
		EXCEPTION("Tried to active null AbilityDef!");
		return;
	}

	em->GetCommandProcessor().UseAbility(player, selection.GetEntities(), *ability);
}

void GameViewContext::ActivateAbility(const AbilityDef* ability, EntityId target)
{
	if (selection.size() == 0)
		return;

	if (ability == nullptr) {
		EXCEPTION("Tried to active null AbilityDef!");
		return;
	}


	em->GetCommandProcessor().UseAbility(player, selection.GetEntities(), *ability, target);
}

void GameViewContext::ActivateAbility(const AbilityDef* ability, Vector2Int16 position)
{
	if (selection.size() == 0)
		return;

	if (ability == nullptr) {
		EXCEPTION("Tried to active null AbilityDef!");
		return;
	}

	em->GetCommandProcessor().UseAbility(player, selection.GetEntities(), *ability, position);
}

void GameViewContext::ActivateAbility(const AbilityDef* ability, const UnitDef& produce)
{
	if (selection.size() == 0)
		return;

	if (ability == nullptr) {
		EXCEPTION("Tried to active null AbilityDef!");
		return;
	}

	em->GetCommandProcessor().UseAbility(player, selection.GetEntities(), *ability, produce);
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

void GameViewContext::ActivateCurrentAbility(const UnitDef& produce)
{
	ActivateAbility(currentAbility, produce);
	CancelTargetSelection();
}

void GameViewContext::CancelBuildQueue(int queuePos)
{
	GetEntityManager().GetCommandProcessor().CancelBuildQueue(player, selection.GetEntities(), queuePos);
}

void GameViewContext::SelectAbilityTarget(const AbilityDef& def)
{
	currentAbility = &def;
	IsTargetSelectionMode = true;

	//if (!def.TargetingData.HasTargetSelection)
	//{
	//	ActivateCurrentAbility();
	//	CancelTargetSelection();
	//}
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

void GameViewContext::NewUnitMarker(EntityId target)
{
	abilityTargetMarkerTimer = AbilityUnitTargetMarkerTimer;
	abilityTarget = target;
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
		bool multiSelection = false;

		for (EntityId id : castResults) {
			if (UnitEntityUtil::IsAlly(player, id) &&
				!em.UnitArchetype.DataComponents.GetComponent(id).isBuilding) {
				multiSelection = true;
				break;
			}
		}

		static std::vector<EntityId> finalSelection;

		if (multiSelection) {
			finalSelection.clear();
			for (EntityId id : castResults) {
				if (UnitEntityUtil::IsAlly(player, id) &&
					!em.UnitArchetype.DataComponents.GetComponent(id).isBuilding) {
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
			PlayUnitSelectedAudio();

		selectionColor = GetAlliedUnitColor(selection[0]);

		CancelTargetSelection();
	}

}

void GameViewContext::SelectUnitAtPosition(Vector2Int16 position)
{
	EntityId id = GetUnitAtPosition(position);

	if (id != Entity::None) {
		selection.clear();
		selection.AddEntity(id);

		if (HasSelectionControl())
			PlayUnitSelectedAudio();

		selectionColor = GetAlliedUnitColor(selection[0]);

		CancelTargetSelection();
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

void GameViewContext::PlayUnitSelectedAudio()
{
	EntityId id = GetPriorityUnitSelected();

	if (id == Entity::None) {
		EXCEPTION("Tried to play audio for entity id none in group!");
	}

	const auto& def = *GetEntityManager().UnitArchetype.UnitComponents.GetComponent(id).def;

	GetEntityManager().GetSoundSystem().PlayUnitSelect(id, def);

}
void GameViewContext::PlayUnitCommandAudio() {
	EntityId id = GetPriorityUnitSelected();

	if (id == Entity::None) {
		EXCEPTION("Tried to play audio for entity id none in group!");
	}

	const auto& def = *GetEntityManager().UnitArchetype.UnitComponents.GetComponent(id).def;

	GetEntityManager().GetSoundSystem().PlayUnitCommand(id, def);
}

const RaceDef& GameViewContext::GetPlayerRaceDef() const
{
	auto& info = GetPlayerInfo();

	return *GameDatabase::instance->GetRace(info.race);
}

const PlayerInfo& GameViewContext::GetPlayerInfo() const
{
	return GetEntityManager().GetPlayerSystem().GetPlayerInfo(player);
}

Color GameViewContext::GetAlliedUnitColor(EntityId id) const {
	if (UnitEntityUtil::IsAlly(player, id))
		return Colors::UIDarkGreen;

	if (UnitEntityUtil::IsEnemy(player, id))
		return Colors::UIDarkRed;

	return Colors::UIDarkYellow;
}