#pragma once

#include "../Data/RaceDef.h"
#include "../Data/UnitDef.h"
#include "../Color.h"
#include "../MathLib.h"
#include "Entity.h"
#include "Component.h"

#include <cstring>
#include <bitset>
#include <vector>

class EntityManager;

struct PlayerVision {
	typedef std::bitset<32> VisionCell;

	std::vector<Circle16> visible;
	std::vector<VisionCell> knoweldge;
	std::vector<VisionCell> visibility;
	Vector2Int16 gridSize;

	inline void ClearVisible() {
		visible.clear();
		memset(visibility.data(), 0, sizeof(VisionCell) * visibility.size());
	}

	inline void SetGridSize(Vector2Int16 size) {
		gridSize = size;
		int s = (size.x >> 5) * (size.y);
		knoweldge.clear();
		knoweldge.reserve(s);
		visibility.clear();
		visibility.reserve(s);
		for (int i = 0; i < s; ++i) {
			knoweldge.push_back({ 0 });
			visibility.push_back({ 0 });
		}
	}

	inline int GetIndex(Vector2Int16 pos) const {
		return (pos.x >> 5) + ((pos.y * (int)gridSize.x) >> 5);

	}

	inline bool IsKnown(Vector2Int16 pos) const {
		if (pos.x < 0 || pos.y < 0 || pos.x >= gridSize.x || pos.y >= gridSize.y)
			return false;

		int i = (pos.x >> 5) + ((pos.y * (int)gridSize.x) >> 5);

		const VisionCell& b = knoweldge[i];
		return b.test((pos.x % 32));
	}

	inline void SetKnown(Vector2Int16 pos, bool known) {
		int i = (pos.x >> 5) + ((pos.y * (int)gridSize.x) >> 5);

		VisionCell& b = knoweldge[i];
		b.set((pos.x % 32), known);
	}

	inline bool IsVisible(Vector2Int16 pos) const {
		if (pos.x < 0 || pos.y < 0 || pos.x >= gridSize.x || pos.y >= gridSize.y)
			return false;

		int i = (pos.x >> 5) + ((pos.y * (int)gridSize.x) >> 5);

		const VisionCell& b = visibility[i];
		return b.test((pos.x % 32));
	}

	inline void SetVisible(Vector2Int16 pos, bool known) {
		int i = (pos.x >> 5) + ((pos.y * (int)gridSize.x) >> 5);

		VisionCell& b = visibility[i];
		b.set((pos.x % 32), known);
	}
	inline void SetExplored(const Vector2Int16& pos) {
		int i = (pos.x >> 5) + ((pos.y * (int)gridSize.x) >> 5);
		int j = pos.x % 32;

		visibility[i].set(j);
		knoweldge[i].set(j);
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

	void UpdatePlayerVision(PlayerVision& vision);
public:
	void SetSize(Vector2Int16 size);

	PlayerId AddPlayer(const RaceDef& race, Color color);

	const PlayerInfo& GetPlayerInfo(PlayerId id) const;
	const PlayerVision& GetPlayerVision(PlayerId id) const;

	void AddMinerals(PlayerId player, int minerals);
	void AddGas(PlayerId player, int gas);

	void UpdatePlayerUnits(const EntityManager& em);
};