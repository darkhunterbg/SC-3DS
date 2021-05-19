#include "PlayerSystem.h"
#include "EntityManager.h"
#include "../Profiler.h"
#include "../MathLib.h"
#include <algorithm>

static constexpr const uint8_t TileVisibilityTimer = 50;

// ================== Player Vision ==================

void PlayerVision::SetGridSize(Vector2Int16 size) {
	gridSize = size;
	int s = (size.x >> 5) * (size.y);
	knoweldge.clear();
	knoweldge.reserve(s);
	visibility.clear();
	visibility.reserve(s);
	visibilityCountdown.clear();
	visibilityCountdown.reserve(s * 32);
	for (int i = 0; i < s; ++i) {
		knoweldge.push_back({ 0 });
		visibility.push_back({ 0 });

		for (int j = 0; j < 32; ++j)
			visibilityCountdown.push_back(0);
	}
}

// ================== Player System ==================

void PlayerSystem::SetSize(Vector2Int16 size)
{
	gridSize.x = size.x / 32;
	gridSize.y = size.y / 32;
}

PlayerId PlayerSystem::AddPlayer(const RaceDef& race, Color color)
{
	PlayerId id = players.size();

	players.push_back(PlayerInfo(Color32(color), race.Type, id));
	playerVision.push_back(new PlayerVision());
	playerVision[id]->SetGridSize(gridSize);

	return id;
}

const PlayerInfo& PlayerSystem::GetPlayerInfo(PlayerId id) const
{
	return players[id];
}

const PlayerVision& PlayerSystem::GetPlayerVision(PlayerId id) const
{
	return *playerVision[id];
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
	for (PlayerVision* vision : playerVision) {
		vision->ClearVisible();
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

		PlayerId owner = em.UnitArchetype.OwnerComponents.GetComponent(id);
		const auto& data = em.UnitArchetype.DataComponents.GetComponent(id);
		Vector2Int16 position = em.PositionComponents.GetComponent(id);
		position.x = position.x >> (5);
		position.y = position.y >> (5);

		playerVision[owner]->ranges.push_back({ position, data.visiion });
	}
}
static int playerUpdate = 0;


static std::array<uint32_t, 128 * 128 / 32> buffer;

bool PlayerSystem::UpdateNextPlayerVision(int players) {
	SectionProfiler p("UpdatePlayerVision");

	int max = std::min((int)playerVision.size(), playerUpdate + players);
	for (int i = playerUpdate; i < max; ++i) {
		PlayerVision& vision = *playerVision[i];


		UpdatePlayerVision(vision);


	/*	SectionProfiler p2("UpdateVisionTimers");*/


		int max = vision.visibilityCountdown.size();
		for (int t = 0; t < max; ++t)
		{
			uint8_t& countdown = vision.visibilityCountdown[t];

			if (countdown > 0) {
				--countdown;
				Vector2Int16 p = Vector2Int16(t % gridSize.x, t / gridSize.x);
				vision.SetExplored(p);
			}
		}


		p.Submit();
	}


	playerUpdate += players;
	if (playerUpdate >= playerVision.size()) {
		playerUpdate = 0;
	}

	return playerUpdate == 0;
}


void PlayerSystem::UpdatePlayerVision(PlayerVision& vision) {
	memset(buffer.data(), 0, buffer.size() * sizeof(uint32_t));

	int max = vision.visibility.size();
	int i = 0;
	for (const Circle16& circle : vision.ranges) {
		Vector2Int16 min = circle.position - Vector2Int16(circle.size);
		Vector2Int16 max = circle.position + Vector2Int16(circle.size);


		min.x = std::max((short)0, min.x);
		min.y = std::max((short)0, min.y);
		max.x = std::min((short)(gridSize.x), max.x);
		max.y = std::min((short)(gridSize.y), max.y);
		
		int size = circle.size * circle.size;

		if (min != circle.position - Vector2Int16(circle.size) ||
			max != circle.position + Vector2Int16(circle.size))
		{
			for (short y = min.y; y <= max.y; ++y) {
				for (short x = min.x; x <= max.x; ++x) {

					Vector2Int16 p = Vector2Int16(x, y);

					Vector2Int16 r = circle.position - p;
					if (r.LengthSquaredInt() < size) {

						vision.visibilityCountdown[p.x + (p.y << 7)]
							= TileVisibilityTimer;
					}
				}
			}
		}
		else {
			for (short y = min.y; y < circle.position.y; ++y) {
				for (short x = min.x; x < circle.position.x; ++x) {

					Vector2Int16 p = Vector2Int16(x, y);
					Vector2Int16 r = circle.position - p;
					if (r.LengthSquaredInt() < size) {
						vision.visibilityCountdown[p.x + (p.y << 7)]
							= TileVisibilityTimer;

						p.x += r.x << 1;
						vision.visibilityCountdown[p.x + (p.y << 7)]
							= TileVisibilityTimer;

						p.y += r.y << 1;
						vision.visibilityCountdown[p.x + (p.y << 7)]
							= TileVisibilityTimer;
						p.x -= r.x << 1;
						vision.visibilityCountdown[p.x + (p.y << 7)]
							= TileVisibilityTimer;

					}

				}
			}

			Vector2Int16 p = circle.position;
			vision.visibilityCountdown[p.x + (p.y << 7)]
				= TileVisibilityTimer;
		}

	}

}