#pragma once

#include "../Data/RaceDef.h"
#include "../Data/UnitDef.h"
#include "../Color.h"
#include "../MathLib.h"
#include "Entity.h"
#include "Component.h"

#include <vector>

class EntityManager;

struct PlayerVision {
	std::vector<Circle16> visible;

	inline void clear()  {
		visible.clear();
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

public:
	PlayerId AddPlayer(const RaceDef& race, Color color);

	const PlayerInfo& GetPlayerInfo(PlayerId id) const;
	const PlayerVision& GetPlayerVision(PlayerId id) const;

	void AddMinerals(PlayerId player, int minerals);
	void AddGas(PlayerId player, int gas);

	void UpdatePlayerUnits(const EntityManager& em);
};