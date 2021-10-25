#pragma once

#include "../Entity/PlayerSystem.h"
#include "../Assets.h"

class ResourceBar {

	struct Resource {
		int shown = 0;
		int target = 0;
		const ImageFrame* icon = nullptr;
	};

	struct Supply {
		int current = 0;
		int max = 0;
		const ImageFrame* icon = nullptr;
		bool enabled = false;
	};

	char _textBuffer[128];
	Resource _minerals = {};
	Resource _gas = {};
	Supply _supply[3];

	void UpdateResourceDiff(ResourceBar::Resource& r);
	void DrawResource(Vector2Int pos, Resource& resource);
	void DrawSupply(Vector2Int pos, Supply& supply);
public:
	ResourceBar();
	void UpdatePlayerInfo(const PlayerInfo& player, bool instant = false);
	void Draw(const PlayerInfo& player);
};