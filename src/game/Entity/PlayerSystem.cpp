#include "PlayerSystem.h"
#include "EntityManager.h"
#include "../Profiler.h"
#include "../MathLib.h"
#include <algorithm>

void PlayerSystem::SetSize(Vector2Int16 size)
{
	gridSize.x = size.x / 32;
	gridSize.y = size.y / 32;
}


PlayerId PlayerSystem::AddPlayer(const RaceDef& race, Color color)
{
	PlayerId id = players.size();

	players.push_back(PlayerInfo(Color32(color), race.Type, id));
	playerVision.push_back(PlayerVision());
	playerVision[id].SetGridSize(gridSize);

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

static bool CircleSort(const  Circle16& a, Circle16& b) {
	return a.position.LengthSquaredInt() < b.position.LengthSquaredInt();
}

void PlayerSystem::UpdatePlayerUnits(const EntityManager& em) {

	for (PlayerVision& vision : playerVision) {
		vision.ClearVisible();
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

	SectionProfiler p("PlayerCollectVision");

	for (EntityId id : em.UnitArchetype.Archetype.GetEntities()) {
		// TODO: store in components to avoid constant calculations
		PlayerId owner = em.UnitArchetype.OwnerComponents.GetComponent(id);
		const auto& data = em.UnitArchetype.DataComponents.GetComponent(id);
		Vector2Int16 position = em.PositionComponents.GetComponent(id);
		position.x = position.x >> (5);
		position.y = position.y >> (5);

		playerVision[owner].visible.push_back({ position, data.visiion });
	}
	p.Submit();

	SectionProfiler p2("PlayerGenerateVision");

	for (PlayerVision& vision : playerVision) {
		UpdatePlayerVision(vision);

	}
}

static std::vector<Vector2Int16> circleCoord;

static void InitCircle(int radius) {
	if (circleCoord.size() > 0)
		return;

	for (int y = -radius; y <= radius; ++y) {
		for (int x = -radius; x <= radius; ++x) {
			circleCoord.push_back(Vector2Int16(x, y));
		}
	}
}

void PlayerSystem::UpdatePlayerVision(PlayerVision& vision) {
	
	//std::sort(vision.visible.begin(), vision.visible.end(), CircleSort);

	int max = vision.visibility.size();
	int i = 0;
	for (const Circle16& circle : vision.visible) {


		Vector2Int16 min = circle.position - Vector2Int16(circle.size);
		Vector2Int16 max = circle.position + Vector2Int16(circle.size);

		min.x = std::max((short)0, min.x);
		min.y = std::max((short)0, min.y);
		max.x = std::min((short)(gridSize.x - 1), max.x);
		max.y = std::min((short)(gridSize.y - 1), max.y);

		// Circle can be mirrored, walk 4 times Pi/2 points only

		int size = circle.size * circle.size;

		for (short y = min.y; y < max.y; ++y) {
			for (short x = min.x; x < max.x; ++x) {

				Vector2Int16 p = Vector2Int16(x, y);
				int i = vision.GetIndex(p);
				int j = p.x % 32;
				Vector2Int16 r = p - circle.position;

				if (vision.visibility[i].test(j))
					continue;

				if (r.LengthSquaredInt() < size) {
					vision.visibility[i].set(j);
					vision.knoweldge[i].set(j);
				}
			}
		}
	}

}