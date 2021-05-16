#pragma once

#include "../MathLib.h"
#include "../Assets.h"

class EntityManager;
class Camera;

class MapSystem {
private:
	Vector2Int16 mapSize;
	Image minimapTexture = { 0,0 };
	Sprite minimapSprite;

	void DrawMinimap();
	void RenderMinimapTexture();
public:
	MapSystem();

	inline const Sprite& GetMinimapSprite() const {
		return minimapSprite;
	}

	void SetSize(Vector2Int16 size);

	void UpdateMap(EntityManager& em);

	void DrawMap( const Camera& camera);
};