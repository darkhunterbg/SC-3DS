#include "PlayerSystem.h"

PlayerId PlayerSystem::AddPlayer(const RaceDef& race, Color color)
{
	PlayerId id = players.size();

	players.push_back(PlayerInfo(Color4(color), race, id));

	return id;
}

const PlayerInfo& PlayerSystem::GetPlayerInfo(PlayerId id) const
{
	return players[id];
}

void PlayerSystem::AddMinerals(PlayerId i, int minerals)
{
	auto& player = players[i];
	player.minerals += minerals;
	if (player.minerals < 0)
		player.minerals = 0;
}

void PlayerSystem::AddGas(PlayerId i, int gas)
{
	auto& player = players[i];
	player.gas += gas;
	if (player.gas < 0)
		player.gas = 0;
}
