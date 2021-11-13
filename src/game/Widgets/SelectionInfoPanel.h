#pragma once

#include "../Entity/Entity.h"

#include <vector>

class SelectionInfoPanel {
private:
	char _buffer[64];
	const struct RaceDef* _raceDef = nullptr;
	std::vector<EntityId> _temp;

	void DrawUnitInfo(EntityId id);
	void DrawUnitName(EntityId id);
	void DrawUnitDetails(EntityId id);
	void DrawMultiselection(std::vector<EntityId>& selection);

	const struct SoundSetDef* _buttonSound;
public:
	SelectionInfoPanel();
	void Draw(std::vector<EntityId>& selection, const struct RaceDef& skin);
};