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

void PlayerSystem::UpdateVision() {
	SectionProfiler p("UpdateVision");

	for (PlayerVision* vision : playerVision) {
		UpdatePlayerVision(*vision);
	}

	p.Submit();

	SectionProfiler p2("UpdateVisionTimers");

	for (PlayerVision* vision : playerVision) {
		int max = vision->visibilityCountdown.size();
		for (int i = 0; i < max; ++i)
		{
			uint8_t& countdown = vision->visibilityCountdown[i];
			if (countdown > 0) {
				--countdown;
				Vector2Int16 p = Vector2Int16(i % gridSize.x, i / gridSize.x);
				vision->SetExplored(p);
			}
		}
	}
}

void PlayerSystem::UpdatePlayerVision(PlayerVision& vision) {


	//std::sort(vision.visible.begin(), vision.visible.end(), CircleSort);

	int max = vision.visibility.size();
	int i = 0;
	for (const Circle16& circle : vision.ranges) {
		Vector2Int16 min = circle.position - Vector2Int16(circle.size);
		Vector2Int16 max = circle.position + Vector2Int16(circle.size);

		min.x = std::max((short)0, min.x);
		min.y = std::max((short)0, min.y);
		max.x = std::min((short)(gridSize.x), max.x);
		max.y = std::min((short)(gridSize.y), max.y);

		// Circle can be mirrored, walk 4 times Pi/2 points only

		int size = circle.size * circle.size;

		for (short y = min.y; y < max.y; ++y) {
			for (short x = min.x; x < max.x; ++x) {

				Vector2Int16 p = Vector2Int16(x, y);
				Vector2Int16 r = p - circle.position;
				/*	int i = vision.GetIndex(p);
					int j = p.x % 32;
					int k = 1 << j;


					const auto& b = vision.visibility[i];
					if (b & k)
						continue;*/

				if (r.LengthSquaredInt() < size) {

					/*	vision.visibility[i] |= k;
						vision.knoweldge[i] |= k;*/
					uint8_t& coundown = vision.visibilityCountdown[x + y * gridSize.x];
					/*	int timerId = x + y * gridSize.x;
						if (coundown == 0) {
							vision.visibilityTimers.push_back(timerId);
						}*/
					coundown = TileVisibilityTimer;
				}
			}
		}
	}

}