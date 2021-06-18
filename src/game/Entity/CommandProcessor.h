#pragma once

#include "Common.h"
#include "../MathLib.h"
#include "../Entity/Entity.h"
#include "../Data/AbilityDef.h"
#include "../Data/UnitDef.h"

#include <array>
#include <vector>

class EntityManager;

enum class PlayerCommandTargetType : uint8_t  {
	None,
	Entity,
	Position,
	UnitType,
	CancelBuildQueue
};

struct PlayerCommand {
	std::array<EntityId, 12> entities;

	union
	{
		EntityId entity;
		Vector2Int16 position;
		uint32_t itemId;
	} target = { 0 };

	uint32_t frameId;
	PlayerId playerId;
	AbilityId abilityId;
	uint8_t entityCount = 0;
	PlayerCommandTargetType targetType;
};

class CommandProcessor {
private:
	std::vector<PlayerCommand> commands;
	uint32_t frame;
	uint32_t lastExecuted = 0;


	void GenerateCommands(PlayerCommand& cmd, const std::vector<EntityId>& group);
public:
	CommandProcessor() {}
	CommandProcessor(const CommandProcessor&) = delete;
	CommandProcessor& operator=(const CommandProcessor&) = delete;

	void UseAbility(PlayerId player, const std::vector<EntityId>& group, const AbilityDef& ability);
	void UseAbility(PlayerId player,  const std::vector<EntityId>& group, const AbilityDef& ability, Vector2Int16 target);
	void UseAbility(PlayerId player, const std::vector<EntityId>& group, const AbilityDef& ability, EntityId target);
	void UseAbility(PlayerId player, const std::vector<EntityId>& group, const AbilityDef& ability, const UnitDef& target);

	void CancelBuildQueue(PlayerId player, const std::vector<EntityId>& group, uint8_t queuePos);

	void ExecuteQueuedCommands(EntityManager& em);

	void RecordToFile(const char* filename);
	void ReplayFromFile(const char* filename, EntityManager& em);

	inline void SetFrame(uint32_t f){
		frame = f;
	}
};
