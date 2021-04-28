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
private:
	Font font;
	const SpriteAtlas* iconsAtlas;
	const SpriteAtlas* consoleAtlas;
	Texture minimapTexture = nullptr;
	Sprite minimapSprite;
	Vector2 minimapUpscale;

	char textBuffer[128];

	struct Supply {
		int current;
		int max;
	};

	int minerals;
	int gas;
	Supply supply;

	void DrawResource(Sprite icon, Vector2Int pos, const char* text, ...);
	void DrawMinimap(const Camera& camera, const MapSystem& mapSystem);
	void RenderMinimapTexture(const MapSystem& mapSystem);
};