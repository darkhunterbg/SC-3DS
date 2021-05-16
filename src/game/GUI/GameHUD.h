#pragma once

#include "../Assets.h"
#include "../Data/RaceDef.h"

class Camera;
class MapSystem;
class PlayerInfo;

class GameHUD {
public:
	GameHUD(const RaceDef& race);

	void ApplyInput(Camera& camera);
	void UpperScreenGUI();
	void LowerScreenGUI(const Camera& camera, const MapSystem& mapSystem);
	void UpdateInfo(const PlayerInfo& info);

private:
	const RaceDef& race;

	Font font;
	const SpriteAtlas* iconsAtlas;
	const SpriteAtlas* cmdIconsAtlas;
	Image minimapTexture = { 0,0 };
	Sprite minimapSprite;
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

	void DrawResource(Sprite icon, Vector2Int pos, const char* text, ...);
	void DrawMinimap(const Camera& camera, const MapSystem& mapSystem);
	void DrawAbilities();
	void RenderMinimapTexture(const MapSystem& mapSystem);

	static void UpdateResourceDiff(Resource& r);
};