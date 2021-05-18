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
	// Once VisionCell holds 32 cells bits.
	// Set bit means visiblity is ON.
	typedef uint32_t VisionCell;

	Vector2Int16 gridSize;

	std::vector<Circle16> ranges;
	std::vector<VisionCell> knoweldge;
	std::vector<VisionCell> visibility;
	std::vector<uint8_t> visibilityCountdown;
	//std::vector<uint16_t> visibilityTimers;



	inline void ClearVisible() {
		ranges.clear();
		memset(visibility.data(), 0, sizeof(VisionCell) * visibility.size());
	}

	void SetGridSize(Vector2Int16 size);

	inline int GetIndex(Vector2Int16 pos) const {
		return (pos.x >> 5) + ((pos.y * (int)gridSize.x) >> 5);

	}

	inline bool IsKnown(Vector2Int16 pos) const {
		if (pos.x < 0 || pos.y < 0 || pos.x >= gridSize.x || pos.y >= gridSize.y)
			return false;

		int i = (pos.x >> 5) + ((pos.y * (int)gridSize.x) >> 5);

		const VisionCell& b = knoweldge[i];
		return b & (1 << (pos.x % 32));
	}

	inline void SetKnown(Vector2Int16 pos, bool known) {
		int i = (pos.x >> 5) + ((pos.y * (int)gridSize.x) >> 5);

		VisionCell& b = knoweldge[i];
		b |= (1 << (pos.x % 32));
	}

	inline bool IsVisible(Vector2Int16 pos) const {
		if (pos.x < 0 || pos.y < 0 || pos.x >= gridSize.x || pos.y >= gridSize.y)
			return false;

		int i = (pos.x >> 5) + ((pos.y * (int)gridSize.x) >> 5);

		const VisionCell& b = visibility[i];
		return  b & (1 << (pos.x % 32)); 
	}

	inline void SetVisible(Vector2Int16 pos, bool known) {
		int i = (pos.x >> 5) + ((pos.y * (int)gridSize.x) >> 5);

		VisionCell& b = visibility[i];
		b |= (1 << (pos.x % 32));
	}
	inline void SetExplored(const Vector2Int16& pos) {
		int i = (pos.x >> 5) + ((pos.y * (int)gridSize.x) >> 5);
		int j = pos.x % 32;
		int k = 1 << j;

		visibility[i] |= k;
		knoweldge[i] |= k;
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
	std::vector<PlayerVision*> playerVision;

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
	void UpdateVision();
};