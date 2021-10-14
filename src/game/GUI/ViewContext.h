#pragma once

#include "../Entity/PlayerSystem.h"

class ViewContext {
private:
	ViewContext(const ViewContext&) = delete;
	ViewContext& operator=(const ViewContext&) = delete;

	PlayerId _player;

public:

	ViewContext();

	inline PlayerId GetPlayer() const { return _player; }
	inline void SetPlayer(PlayerId player) { _player = player; }

	const PlayerInfo& GetPlayerInfo() const;
};