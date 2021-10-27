#pragma once

#include "../Entity/Entity.h"

#include "../Assets.h"

class UnitPortraitPanel {
private:
	int _portraitId = 0;
	const Image* _noiseImage = nullptr;
	EntityId _unit = Entity::None;
	int _noiseCooldown;

public:
	UnitPortraitPanel();
	void Draw(EntityId unit);
};