#pragma once

#include "../Entity/Entity.h"

#include <vector>


struct RaceDef;

class SelectionInfoPanel {
private:
	char _buffer[64];
	const RaceDef* _raceDef = nullptr;
	std::vector<EntityId> _temp;

	void DrawUnitInfo(EntityId id);
	void DrawUnitName(EntityId id);
	void DrawUnitDetails(EntityId id);
	void DrawMultiselection(std::vector<EntityId>& selection);
public:
	void Draw(std::vector<EntityId>& selection, const RaceDef& skin);
};