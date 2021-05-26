#pragma once

#include "../Assets.h"
#include "../Data/RaceDef.h"
#include "../Color.h"
#include "../Entity/Entity.h"
#include "UnitSelectionConsolePanel.h"
#include "UnitCommandsPanel.h"
#include "GameHUDContext.h"

#include <vector>

class Camera;
class EntityManager;

struct GUIActionMarker {
	Vector2Int16 pos;
	uint8_t timer;
	uint8_t state = 0;
};

class GameHUD {
public:
	GameHUD(EntityManager& em, Vector2Int16 mapSize);

	void UpdateSelection();

	void ApplyInput(Camera& camera);
	void UpperScreenGUI(const Camera& camera);
	void LowerScreenGUI(const Camera& camera);
	

	void NewActionMarker(Vector2Int16 pos);

	GameHUDContext context;

	void SetPlayer(PlayerId player,const RaceDef& race);
private:
	UnitSelectionConsolePanel consolePanel;
	UnitCommandsPanel commandsPanel;

	Font font;
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

	std::vector<GUIActionMarker> markers;



	void DrawMarkers(const Camera& camera);
	void DrawUnitBars(const Camera& camera, const std::vector<EntityId>& selection, EntityManager& em);
	void DrawResource(Sprite icon, Vector2Int pos, Color color, const char* text, ...);
	void DrawMinimap(const Camera& camera);

	void UpdateInfo();

	static void UpdateResourceDiff(Resource& r);
};