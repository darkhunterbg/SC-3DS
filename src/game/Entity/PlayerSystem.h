#pragma once

#include "../Data//RaceDef.h"
#include "../Color.h"

#include <vector>

typedef uint8_t PlayerId;

struct PlayerInfo {

	Color4 color;
	const RaceDef& race;
	PlayerId id;
	int minerals = 0;
	int gas = 0;
	int currentSupplyDoubled = 0;
	unsigned maxSupplyDoubled = 0;

	inline unsigned GetCurrentSupply() const {
		return currentSupplyDoubled >> 2;
	}
	inline unsigned GetMaxSupply() const {
		return maxSupplyDoubled >> 2;
	}

	PlayerInfo(Color4 color, const RaceDef& race, PlayerId id) :
		color(color), race(race), id(id) {}
};

class PlayerSystem {
private:
	std::vector<PlayerInfo> players;
public:
	PlayerId AddPlayer(const RaceDef& race, Color color);

	const PlayerInfo& GetPlayerInfo(PlayerId id) const;

	void AddMinerals(PlayerId player, int minerals);
	void AddGas(PlayerId player, int gas);
};