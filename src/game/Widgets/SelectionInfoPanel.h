#pragma once

#include "../Entity/Entity.h"

#include <vector>


class RaceDef;

class SelectionInfoPanel {
private:
	char _buffer[64];
	const RaceDef* _raceDef = nullptr;

	void DrawUnitInfo(EntityId id);
	void DrawUnitName(EntityId id);
	void DrawUnitDetails(EntityId id);
	
public:
	void Draw(const std::vector<EntityId>& selection, const RaceDef& skin);
};