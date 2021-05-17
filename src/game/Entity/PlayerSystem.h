#pragma once

#include "../Data/RaceDef.h"
#include "../Data/UnitDef.h"
#include "../Color.h"
#include "../MathLib.h"
#include "Entity.h"
#include "Component.h"

#include <bitset>
#include <vector>

class EntityManager;

struct PlayerVision {
	std::vector<Circle16> visible;
	std::vector<std::bitset<32>> knoweldge;
	Vector2Int16 gridSize;

	inline void clear() {
		visible.clear();
	}

	inline void SetGridSize(Vector2Int16 size) {
		gridSize = size;
		int s = (size.x >> 3) * (size.y);
		knoweldge.clear();
		knoweldge.reserve(s);
		for (int i = 0; i < s; ++i) {
			knoweldge.push_back({ 0 });
		}
	}

	inline bool IsKnown(Vector2Int16 pos) const {
		int i = (pos.x >> 3) + ((pos.y * (int)gridSize.x) >> 3);

		const std::bitset < 32 >& b = knoweldge[i];
		return b.test((pos.x % 32));
	}

	inline void SetKnown(Vector2Int16 pos, bool known) {
		int i = (pos.x >> 3) + ((pos.y * (int)gridSize.x) >> 3);

		std::bitset < 32 >& b = knoweldge[i];
		b.set((pos.x % 32), known);
	}
};

struct PlayerInfo {
	Color32 color;

	int16_t minerals = 0;
	int16_t gas = 0;
	int16_t currentSupplyDoubled = 0;
	int16_t maxSupplyDoubled = 0;

	RaceType race;

	PlayerId id;

	inline int GetCurrentSupply() const {
		return currentSupplyDoubled >> 2;
	}
	inline int GetMaxSupply() const {
		return maxSupplyDoubled >> 2;
	}

	PlayerInfo(Color32 color, RaceType race, PlayerId id) :
		color(color), race(race), id(id) {}
};

class PlayerSystem {
private:
	std::vector<PlayerInfo> players;
	std::vector<PlayerVision> playerVision;

	friend class EntityManager;

	Vector2Int16 gridSize;
public:
	void SetSize(Vector2Int16 size);

	PlayerId AddPlayer(const RaceDef& race, Color color);

	const PlayerInfo& GetPlayerInfo(PlayerId id) const;
	const PlayerVision& GetPlayerVision(PlayerId id) const;

	void AddMinerals(PlayerId player, int minerals);
	void AddGas(PlayerId player, int gas);

	void UpdatePlayerUnits(const EntityManager& em);
};