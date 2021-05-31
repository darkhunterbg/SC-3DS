#pragma once

#include "../Assets.h"
#include "../Data/RaceDef.h"
#include "../Color.h"
#include "../Entity/Entity.h"
#include "UnitSelectionConsolePanel.h"
#include "UnitCommandsPanel.h"

#include <vector>

class Camera;
class EntityManager;
class GameViewContext;


class GameHUD {
public:
	GameHUD( Vector2Int16 mapSize);

	void UpperScreenGUI(const Camera& camera, GameViewContext& context);
	void LowerScreenGUI(const Camera& camera, GameViewContext& context);
	
	void Update( Camera& camera, GameViewContext& context);
private:
	UnitSelectionConsolePanel consolePanel;
	UnitCommandsPanel commandsPanel;

	const Font* font;
	const SpriteAtlas* iconsAtlas;

	Vector2 minimapUpscale;

	char textBuffer[128];

	struct Resource {
		int shown;
		int target;
	};

	struct Supply {
		int current;
		int max;
	};

	Resource minerals = { 0,0 };
	Resource gas = { 0,0 };
	Supply supply = { 0,0 };

	void DrawResource(Sprite icon, Vector2Int pos, Color color, const char* text, ...);
	void DrawMinimap(const Camera& camera, GameViewContext& context);

	void UpdateInfo(GameViewContext& context);
	static void UpdateResourceDiff(Resource& r);
};