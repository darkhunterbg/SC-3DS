#pragma once

#include "../Assets.h"


class Camera;
class MapSystem;

class GameHUD {
public:
	GameHUD();

	void ApplyInput(Camera& camera);
	void UpperScreenGUI();
	void LowerScreenGUI(const Camera& camera, const MapSystem& mapSystem);

	inline void AddMinerals(int amount) { minerals.target += amount; }
	inline void SetMinerals(int amount) { minerals.target = minerals.shown = amount; }

	inline void AddGas(int amount) { gas.target += amount; }
	inline void SetGas(int amount) { gas.target = gas.shown = amount; }

	inline void SetMaxSupply(int amount) { supply.max = amount; };
	inline void SetUsedSupply(int amount) { supply.current = amount; }

private:
	Font font;
	const SpriteAtlas* iconsAtlas;
	const SpriteAtlas* cmdIconsAtlas;
	const SpriteAtlas* consoleAtlas;
	Texture minimapTexture = nullptr;
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