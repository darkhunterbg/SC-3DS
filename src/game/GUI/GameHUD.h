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

class GameHUD {
public:
	GameHUD(Vector2Int16 mapSize);

	void UpperScreenGUI(const Camera& camera);
	void LowerScreenGUI(const Camera& camera);

	void Update(Camera& camera);
private:
	UnitSelectionConsolePanel consolePanel;
	UnitCommandsPanel commandsPanel;
	ImageFrame mineralIcon;

	const Font* font;

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

	void DrawResource(const ImageFrame& icon, Vector2Int pos, Color color, const char* text, ...);
	void DrawMinimap(const Camera& camera);

	void UpdateInfo();
	static void UpdateResourceDiff(Resource& r);
};