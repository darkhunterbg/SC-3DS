#include "EntityUtil.h"
#include "CommandProcessor.h"
#include "../Debug.h"
#include "EntityManager.h"

#include "../Data/AbilityDatabase.h"

void CommandProcessor::GenerateCommands(PlayerCommand& cmd, const std::vector<EntityId>& group) {
	for (int i = 0; i < group.size(); ++i) {
		cmd.entities[cmd.entityCount++] = group[i];

		if (cmd.entityCount == cmd.entities.size()) {
			commands.push_back(cmd);
			cmd.entityCount = 0;
		}
	}

	if (cmd.entityCount) {
		commands.push_back(cmd);
	}
}

void CommandProcessor::UseAbility(PlayerId player, const std::vector<EntityId>& group, const AbilityDef& ability)
{
	PlayerCommand cmd;

	cmd.frameId = frame;
	cmd.playerId = player;
	cmd.abilityId = ability.AbilityId;
	cmd.targetType = PlayerCommandTargetType::None;

	GenerateCommands(cmd, group);
}
void CommandProcessor::UseAbility(PlayerId player, const std::vector<EntityId>& group, const AbilityDef& ability, Vector2Int16 target)
{
	PlayerCommand cmd;

	cmd.frameId = frame;
	cmd.playerId = player;
	cmd.abilityId = ability.AbilityId;
	cmd.target.position = target;
	cmd.targetType = PlayerCommandTargetType::Position;

	GenerateCommands(cmd, group);
}
void CommandProcessor::UseAbility(PlayerId player, const std::vector<EntityId>& group, const AbilityDef& ability, EntityId target)
{
	PlayerCommand cmd;

	cmd.frameId = frame;
	cmd.playerId = player;
	cmd.abilityId = ability.AbilityId;
	cmd.target.entity = target;
	cmd.targetType = PlayerCommandTargetType::Entity;

	GenerateCommands(cmd, group);
}

void CommandProcessor::ExecuteQueuedCommands(EntityManager& em)
{
	for (int i = lastExecuted; i < commands.size(); ++i) {
		const PlayerCommand& cmd = commands[i];

		const auto& ability = *AbilityDatabase::Abilities[cmd.abilityId];

		

		switch (cmd.targetType)
		{
		case  PlayerCommandTargetType::None: {
			UnitAIState action = ability.TargetingData.EntitySelectedAction;
			if (action == UnitAIState::Nothing) {
				EXCEPTION("Tried to active ability %s with Nothing action for entity!", ability.Name.data());
				return;
			}

			for (int j = 0; j < cmd.entityCount; ++j) {
				EntityId id = cmd.entities[j];
				UnitEntityUtil::SetAIState(id, action);
			}
			break;
		}
		case  PlayerCommandTargetType::Entity: {
			UnitAIState action = ability.TargetingData.EntitySelectedAction;
			if (action == UnitAIState::Nothing) {
				EXCEPTION("Tried to active ability %s with Nothing action for entity!", ability.Name.data());
				return;
			}

			for (int j = 0; j < cmd.entityCount; ++j) {
				EntityId id = cmd.entities[j];
				UnitEntityUtil::SetAIState(id, action, cmd.target.entity);
			}
			break;
		}
		case  PlayerCommandTargetType::Position: {
			UnitAIState action = ability.TargetingData.PositionSelectedAction;
			if (action == UnitAIState::Nothing) {
				EXCEPTION("Tried to active ability %s with Nothing action for entity!", ability.Name.data());
				return;
			}

			for (int j = 0; j < cmd.entityCount; ++j) {
				EntityId id = cmd.entities[j];
				UnitEntityUtil::SetAIState(id, action, cmd.target.position);
			}
			break;
		}
		}

	}

	lastExecuted = commands.size();
}
