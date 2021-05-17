#include "PlayerSystem.h"
#include "EntityManager.h"


PlayerId PlayerSystem::AddPlayer(const RaceDef& race, Color color)
{
	PlayerId id = players.size();

	players.push_back(PlayerInfo(Color32(color), race.Type, id));
	playerVision.push_back(PlayerVision());

	return id;
}

const PlayerInfo& PlayerSystem::GetPlayerInfo(PlayerId id) const
{
	return players[id];
}

const PlayerVision& PlayerSystem::GetPlayerVision(PlayerId id) const
{
	return playerVision[id];
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

void PlayerSystem::UpdatePlayerUnits(const EntityManager& em) {

	for (PlayerVision& vision : playerVision) {
		vision.clear();
	}

	for (EntityId id : em.UnitArchetype.Archetype.NewEntities()) {
		PlayerId owner = em.UnitArchetype.OwnerComponents.GetComponent(id);
		const auto& data = em.UnitArchetype.DataComponents.GetComponent(id);

		players[owner].currentSupplyDoubled += data.supplyUsage;
		players[owner].maxSupplyDoubled += data.supplyProvides;
	}
	for (EntityId id : em.UnitArchetype.Archetype.RemovedEntities()) {
		PlayerId owner = em.UnitArchetype.OwnerComponents.GetComponent(id);
		const auto& data = em.UnitArchetype.DataComponents.GetComponent(id);

		players[owner].currentSupplyDoubled -= data.supplyUsage;
		players[owner].maxSupplyDoubled -= data.supplyProvides;
	}
	for (EntityId id : em.UnitArchetype.Archetype.GetEntities()) {
		// TODO: store in components to avoid constant calculations
		PlayerId owner = em.UnitArchetype.OwnerComponents.GetComponent(id);
		const auto& data = em.UnitArchetype.DataComponents.GetComponent(id);
		Vector2Int16 position = em.PositionComponents.GetComponent(id);
		position.x = position.x >> (5);
		position.y = position.y >> (5);
		playerVision[owner].visible.push_back({ position, data.visiion });
	}
}
