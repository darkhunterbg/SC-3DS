#pragma once

#include "../Assets.h"
#include "../Data/RaceDef.h"
#include "../Color.h"
#include "../Entity/Entity.h"
#include "UnitSelectionConsolePanel.h"
#include "UnitCommandsPanel.h"

#include <vector>

class Camera;
class MapSystem;
class PlayerInfo;
class EntityManager;

struct GUIActionMarker {
	Vector2Int16 pos;
	uint8_t timer;
	uint8_t state = 0;
};

class GameHUD {
public:
	GameHUD(const RaceDef& race, Vector2Int16 mapSize);


	void UpdateSelection(std::vector<EntityId>& selection);

	void ApplyInput(Camera& camera);
	void UpperScreenGUI(const Camera& camera, const std::vector<EntityId>& selection, EntityManager& em);
	void LowerScreenGUI(const Camera& camera, const std::vector<EntityId>& selection, EntityManager& em);
	void UpdateInfo(const PlayerInfo& info);

	void NewActionMarker(Vector2Int16 pos);

private:
	const RaceDef& race;
	UnitSelectionConsolePanel consolePanel;
	UnitCommandsPanel commandsPanel;

	Font font;
	const SpriteAtlas* iconsAtlas;
	const SpriteAtlas* cmdIconsAtlas;

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

	std::vector<GUIActionMarker> markers;

	void DrawMarkers(const Camera& camera);
	void DrawUnitBars(const Camera& camera, const std::vector<EntityId>& selection, EntityManager& em);
	void DrawResource(Sprite icon, Vector2Int pos, Color color, const char* text, ...);
	void DrawMinimap(const Camera& camera,  MapSystem& mapSystem);

	static void UpdateResourceDiff(Resource& r);
};