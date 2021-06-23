#include "EntityUtil.h"
#include "CommandProcessor.h"
#include "../Debug.h"
#include "EntityManager.h"
#include "../Data/GameDatabase.h"


#include "../Platform.h"

#include <stdio.h>


// TODO: command alloc with limits to hardware
// static constexpr const unsigned MaxCommands = 30 * 1024; // Around 1MB 

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
	cmd.abilityId = ability.Id;
	cmd.targetType = PlayerCommandTargetType::None;

	GenerateCommands(cmd, group);
}
void CommandProcessor::UseAbility(PlayerId player, const std::vector<EntityId>& group, const AbilityDef& ability, Vector2Int16 target)
{
	PlayerCommand cmd;

	cmd.frameId = frame;
	cmd.playerId = player;
	cmd.abilityId = ability.Id;
	cmd.target.position = target;
	cmd.targetType = PlayerCommandTargetType::Position;

	GenerateCommands(cmd, group);
}
void CommandProcessor::UseAbility(PlayerId player, const std::vector<EntityId>& group, const AbilityDef& ability, EntityId target)
{
	PlayerCommand cmd;

	cmd.frameId = frame;
	cmd.playerId = player;
	cmd.abilityId = ability.Id;
	cmd.target.entity = target;
	cmd.targetType = PlayerCommandTargetType::Entity;

	GenerateCommands(cmd, group);
}

void CommandProcessor::UseAbility(PlayerId player, const std::vector<EntityId>& group, const AbilityDef& ability, const UnitDef& target)
{
	PlayerCommand cmd;

	cmd.frameId = frame;
	cmd.playerId = player;
	cmd.abilityId = ability.Id;
	cmd.target.itemId = target.Id;
	cmd.targetType = PlayerCommandTargetType::UnitType;

	GenerateCommands(cmd, group);
}

void CommandProcessor::CancelBuildQueue(PlayerId player, const std::vector<EntityId>& group, uint8_t queuePos)
{
	PlayerCommand cmd;

	cmd.frameId = frame;
	cmd.playerId = player;
	cmd.abilityId = 0;
	cmd.target.itemId = queuePos;
	cmd.targetType = PlayerCommandTargetType::CancelBuildQueue;

	GenerateCommands(cmd, group);
}

void CommandProcessor::ExecuteQueuedCommands(EntityManager& em)
{
	for (int i = lastExecuted; i < commands.size(); ++i) {
		const PlayerCommand& cmd = commands[i];

		if (cmd.targetType == PlayerCommandTargetType::CancelBuildQueue) {
			for (int j = 0; j < cmd.entityCount; ++j) {
				EntityId id = cmd.entities[j];
			
				em.GetUnitSystem().DequeueItem(id, cmd.target.itemId, em);
			}
			continue;
		}

		const auto& ability = *GameDatabase::instance->GetAbility(cmd.abilityId);

		
		switch (cmd.targetType)
		{
		case  PlayerCommandTargetType::None: {
			UnitAIState action = ability.Data.EntitySelectedAction;
			if (action == UnitAIState::Nothing) {
				EXCEPTION("Tried to active ability %s with Nothing action no target!", ability.Name.data());
				return;
			}

			for (int j = 0; j < cmd.entityCount; ++j) {
				EntityId id = cmd.entities[j];
				UnitEntityUtil::SetAIState(id, action);
			}
			break;
		}
		case  PlayerCommandTargetType::Entity: {
			UnitAIState action = ability.Data.EntitySelectedAction;
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
			UnitAIState action = ability.Data.PositionSelectedAction;
			if (action == UnitAIState::Nothing) {
				EXCEPTION("Tried to active ability %s with Nothing action for position!", ability.Name.data());
				return;
			}

			for (int j = 0; j < cmd.entityCount; ++j) {
				EntityId id = cmd.entities[j];
				UnitEntityUtil::SetAIState(id, action, cmd.target.position);
			}
			break;
		}
		case  PlayerCommandTargetType::UnitType: {
			//const UnitDef& unit = *UnitDatabase::Units[cmd.target.itemId];

			for (int j = 0; j < cmd.entityCount; ++j) {
				EntityId id = cmd.entities[j];

				//em.GetUnitSystem().EnqueueBuildUpdateCheck(id,unit);

			}
			break;
		}
		}

	}

	lastExecuted = commands.size();
}

void CommandProcessor::RecordToFile(const char* filename)
{
	std::string path = Platform::GetUserDirectory() + filename;


	FILE* f = fopen(path.data(), "w+b");

	if (f == nullptr)
		EXCEPTION("Failed to open file for write %s", path.data());

	int size = commands.size();
	if (fwrite(&size, sizeof(int), 1, f) != 1)
		EXCEPTION("Failed to write to %s", path.data());

	int total = fwrite(commands.data(), sizeof(PlayerCommand), commands.size(), f);
	if (total != commands.size())
		EXCEPTION("Failed to write to %s", path.data());

	fclose(f);
}

void CommandProcessor::ReplayFromFile(const char* filename, EntityManager& em)
{
	std::string path = Platform::GetUserDirectory() + filename;

	FILE* f = fopen(path.data(), "rb");

	if (f == nullptr)
		EXCEPTION("Failed to open file for read %s", path.data());

	commands.clear();

	int size = 0;

	if (fread(&size, sizeof(int), 1, f) != 1)
		EXCEPTION("Failed to read from %s", path.data());

	PlayerCommand* buffer = new PlayerCommand[size];

	int total = fread(buffer, sizeof(PlayerCommand), size, f);
	if (total != size)
		EXCEPTION("Failed to read from %s", path.data());


	fclose(f);

	frame = 0;
	lastExecuted = 0;

	if (size > 0)
	{
		uint32_t latest = buffer[size - 1].frameId;

		int counter = 0;

		while (latest != frame)
		{
			while (buffer[counter].frameId != frame)
			{
				em.FullUpdate();
			}

			for (; counter < size; ++counter) {
				if (buffer[counter].frameId == frame) {
					commands.push_back(buffer[counter]);
				}
				else
					break;
			}
		}

	}

	em.GetSoundSystem().ClearAudio(em);

	delete[] buffer;
}

