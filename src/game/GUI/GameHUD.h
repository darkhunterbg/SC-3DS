#pragma once

#include "../Assets.h"
#include "../Data/RaceDef.h"
#include "../Color.h"

#include <vector>

class Camera;
class MapSystem;
class PlayerInfo;

struct GUIActionMarker {
	Vector2Int16 pos;
	uint8_t timer;
	uint8_t state = 0;
};

class GameHUD {
public:
	GameHUD(const RaceDef& race, Vector2Int16 mapSize);

	void ApplyInput(Camera& camera);
	void UpperScreenGUI(const Camera& camera);
	void LowerScreenGUI(const Camera& camera, MapSystem& mapSystem);
	void UpdateInfo(const PlayerInfo& info);

	void NewActionMarker(Vector2Int16 pos);

private:
	const RaceDef& race;

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

	void DrawResource(Sprite icon, Vector2Int pos, Color color, const char* text, ...);
	void DrawMinimap(const Camera& camera,  MapSystem& mapSystem);
	void DrawAbilities();

	static void UpdateResourceDiff(Resource& r);
};