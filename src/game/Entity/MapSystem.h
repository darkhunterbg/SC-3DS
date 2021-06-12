#pragma once

#include "../MathLib.h"
#include "../Assets.h"
#include "../Color.h"
#include "Component.h"

#include "../Engine/GraphicsPrimitives.h"

#include <vector>

class EntityManager;
class Camera;
class PlayerVision;
struct EntityChangedData;

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

	RenderSurface minimapTexture = { 0 };
	RenderSurface minimapFowTexture = { 0 };
	RenderSurface minimapTerrainTexture = { 0};

	RenderSurface fogOfWarTexture = { 0 };
	RenderSurface fowDownscaleTexture = { 0 };

	short minimapTextureSize = 0;

	MinimapData minimapData;

	std::vector<EntityId> removedEntities;
	std::vector<EntityId> addedEntities;

	std::vector<EntityId> scratch;
	std::vector<EntityId> scratch2;

	static void InitFowVisibleEntitiesJob(int start, int end);

	void GenerateMinimapTerrainTexture();
	void RedrawMinimapFogOfWar(const PlayerVision& vision);

	void UpdateVisibleRenderEntities(EntityManager& em);
	void UpdateVisibleRenderUnits(EntityManager& em);

	void UpdateFowVisibleUnits(EntityManager& em);

	void UpdateMapObjectPositions(EntityManager& em, const EntityChangedData& changed);
public:
	bool FogOfWarVisible = true;
	PlayerId ActivePlayer = 1;

	void SetSize(Vector2Int16 size);

	void UpdateMap(EntityManager& em, const EntityChangedData& changed);

	void DrawMap(const Camera& camera);

	void DrawFogOfWar(const Camera& camera);

	void DrawMinimap(Rectangle dst);

	void RedrawMinimap(EntityManager& em);

	void DrawGrid(const Camera& camera);

	void UpdateVisibleEntities(EntityManager& em);
};