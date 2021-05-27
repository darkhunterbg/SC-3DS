#include "PlayerSystem.h"
#include "EntityManager.h"
#include "../Profiler.h"
#include "../MathLib.h"
#include "../Job.h"
#include <algorithm>

static constexpr const uint8_t TileVisibilityTimer = 40;

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

PlayerSystem::PlayerSystem() {
	
}

void PlayerSystem::SetSize(Vector2Int16 size)
{
	gridSize.x = size.x / 32;
	gridSize.y = size.y / 32;


	players.clear();

	// Add neutral player

	PlayerId id = 0;

	players.push_back(PlayerInfo(Color32(Colors::SCNeutral), RaceType::Neutral, id));
	playerVision.push_back(new PlayerVision());
	playerVision[id]->SetGridSize(gridSize);
}

PlayerId PlayerSystem::AddPlayer(const RaceDef& race, Color color)
{
	PlayerId id = players.size();

	players.push_back(PlayerInfo(Color32(color), race.Type, id));
	playerVision.push_back(new PlayerVision());
	playerVision[id]->SetGridSize(gridSize);

	return id;
}
void PlayerSystem::SetMapKnown(PlayerId player) {
	PlayerVision& vision = *playerVision[player];
	memset(vision.knoweldge.data(), 0xFF, sizeof(PlayerVision::VisionCell) * vision.knoweldge.size());
}

const PlayerInfo& PlayerSystem::GetPlayerInfo(PlayerId id) const
{
	return players[id];
}

const PlayerVision& PlayerSystem::GetPlayerVision(PlayerId id) const
{
	if (playerVision.size() <= id)
		EXCEPTION("Player with id %i/%i not found", id, playerVision.size());

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
		vision->ranges.clear();
	}

	for (EntityId id : em.UnitArchetype.Archetype.NewEntities()) {
		PlayerId owner = em.UnitArchetype.OwnerComponents.GetComponent(id);
		const auto& data = em.UnitArchetype.DataComponents.GetComponent(id);

		players[owner].usedSupplyDoubled += data.supplyUsage;
		players[owner].providedSupplyDoubled += data.supplyProvides;
	}
	for (EntityId id : em.UnitArchetype.Archetype.RemovedEntities()) {
		PlayerId owner = em.UnitArchetype.OwnerComponents.GetComponent(id);
		const auto& data = em.UnitArchetype.DataComponents.GetComponent(id);

		players[owner].usedSupplyDoubled -= data.supplyUsage;
		players[owner].providedSupplyDoubled -= data.supplyProvides;
	}

	for (EntityId id : em.UnitArchetype.Archetype.GetEntities()) {

		PlayerId owner = em.UnitArchetype.OwnerComponents.GetComponent(id);
		const auto& data = em.UnitArchetype.DataComponents.GetComponent(id);
		Vector2Int16 position = em.PositionComponents.GetComponent(id);
		position.x = position.x >> (5);
		position.y = position.y >> (5);

		playerVision[owner]->ranges.push_back({ position, data.vision });
	}
}

static PlayerSystem* s;

void PlayerSystem::UpdateNextPlayerVisionJob(int start, int end) {
	for (int i = start; i < end; ++i) {
		PlayerVision& vision = *s->playerVision[i];
		s->UpdatePlayerVision(vision);
		s->UpdatePlayerVisionTimers(vision);
	}
}

bool PlayerSystem::UpdateNextPlayerVision(int players) {
	s = this;
	
	int max = std::min((int)playerVision.size(), playerUpdate + players);

	JobSystem::RunJob(max - playerUpdate, 1, UpdateNextPlayerVisionJob);

	playerUpdate += players;
	if (playerUpdate >= playerVision.size()) {
		playerUpdate = 0;
	}

	return playerUpdate == 0;
}

void PlayerSystem::UpdatePlayerVisionTimers(PlayerVision& vision) {
	const int bitshift = std::log2(vision.gridSize.x);
	const Vector2Int16 gridSize = vision.gridSize;
	const int gridSizeBuckets = gridSize.x >> 5;

	int bucketMax = (gridSize.x >> 5) * (gridSize.y >> 5);

	for (int b = 0; b < bucketMax; ++b) {
		if (!vision.timerBuckets.test(b))
			continue;

		int timers = 0;

		Vector2Int min;
		min.x = (b % gridSizeBuckets) << 5;
		min.y = (b / gridSizeBuckets) << 5;

		for (int y = min.y; y < min.y + 32; ++y) {
			for (int x = min.x; x < min.x + 32; ++x) {
				int t = x + y * gridSize.y;

				uint8_t& countdown = vision.visibilityCountdown[t];

				if (countdown > 0) {
					--countdown;
					Vector2Int16 p = Vector2Int16(t % gridSize.x, t >> bitshift);
					vision.SetExplored(p);
					timers++;
				}
			}
		}

		vision.timerBuckets.set(b, timers);
	}
}

void PlayerSystem::UpdatePlayerVision(PlayerVision& vision) {
	// WARN: bitshift will not work when map is not power of 2 (192, for example)

	vision.ClearVisibility();

	const int bitshift = std::log2(vision.gridSize.x);
	const Vector2Int16 gridSize = vision.gridSize;
	const int gridSizeBuckets = gridSize.x >> 5;
	const int bucketBitshift = std::log2(gridSizeBuckets);

	int end = vision.ranges.size();

	for (int i = 0; i < end; ++i) {
		const Circle16& circle = vision.ranges[i];

		Vector2Int16 min = circle.position - Vector2Int16(circle.size);
		Vector2Int16 max = circle.position + Vector2Int16(circle.size);

		min.x = std::max((short)0, min.x);
		min.y = std::max((short)0, min.y);
		max.x = std::min((short)(gridSize.x - 1), max.x);
		max.y = std::min((short)(gridSize.y - 1), max.y);

		Vector2Int16 bucketMin = min >> 5;
		bucketMin.y = bucketMin.y << bucketBitshift;
		Vector2Int16 bucketMax = (max >> 5);
		bucketMax.y = bucketMax.y << bucketBitshift;

		vision.timerBuckets.set(bucketMin.x + bucketMin.y);
		vision.timerBuckets.set(bucketMax.x + bucketMin.y);
		vision.timerBuckets.set(bucketMin.x + bucketMax.y);
		vision.timerBuckets.set(bucketMax.x + bucketMax.y);

		int size = circle.size * circle.size;

		if (min != circle.position - Vector2Int16(circle.size) ||
			max != circle.position + Vector2Int16(circle.size))
		{
			for (short y = min.y; y <= max.y; ++y) {
				for (short x = min.x; x <= max.x; ++x) {

					Vector2Int16 p = Vector2Int16(x, y);
					Vector2Int16 r = circle.position - p;
					if (r.LengthSquaredInt() < size) {

						vision.visibilityCountdown[p.x + (p.y << bitshift)]
							= TileVisibilityTimer;
					}
				}
			}
		}
		else {
			for (short y = min.y; y <= circle.position.y; ++y) {
				for (short x = min.x; x <= circle.position.x; ++x) {

					Vector2Int16 p = Vector2Int16(x, y);
					Vector2Int16 r = circle.position - p;
					if (r.LengthSquaredInt() < size) {
						vision.visibilityCountdown[p.x + (p.y << bitshift)]
							= TileVisibilityTimer;

						p.x += r.x << 1;
						vision.visibilityCountdown[p.x + (p.y << bitshift)]
							= TileVisibilityTimer;

						p.y += r.y << 1;
						vision.visibilityCountdown[p.x + (p.y << bitshift)]
							= TileVisibilityTimer;
						p.x -= r.x << 1;
						vision.visibilityCountdown[p.x + (p.y << bitshift)]
							= TileVisibilityTimer;

					}
				}
			}
		}
	}
	//JobSystem::RunJob(vision.ranges.size(), vision.ranges.size(), UpdatePlayerVisionJob);
}