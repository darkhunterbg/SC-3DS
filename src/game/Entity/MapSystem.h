#pragma once

#include "../MathLib.h"
#include "../Assets.h"
#include "../Color.h"
#include "Component.h"

#include <vector>

class EntityManager;
class Camera;
class PlayerVision;

class MapSystem {
	struct MinimapData {
		std::vector<Rectangle16> dst;
		std::vector<Color32> color;

		inline void clear() {
			dst.clear();
			color.clear();
		}

		inline size_t size() const { return dst.size(); }
	};
private:

	Vector2Int16 mapSize;
	Vector2Int16 gridSize;
	Image minimapTexture = { 0,0 };
	Image minimapFowTexture = { 0,0 };
	Image minimapTerrainTexture = { 0,0 };
	Image fogOfWarTexture = { 0,0 };

	short minimapTextureSize = 0;

	MinimapData minimapData;

	std::vector<EntityId> removedEntities;
	std::vector<EntityId> addedEntities;

	void GenerateMiniampTerrainTexture();
	void RenderMinimapFogOfWar(const PlayerVision& vision);

	void UpdateVisibleRenderEntities(EntityManager& em);
	void UpdateVisibleRenderUnits(EntityManager& em);
public:
	bool FogOfWarVisible = true;
	PlayerId ActivePlayer = 0;

	void SetSize(Vector2Int16 size);

	void UpdateMap(EntityManager& em);

	void DrawMap(const Camera& camera);

	void DrawFogOfWar(const Camera& camera);

	void DrawMinimap(Rectangle dst);

	void RedrawMinimap(EntityManager& em);

	void DrawGrid(const Camera& camera);

	void UpdateVisibleEntities(EntityManager& em);
};