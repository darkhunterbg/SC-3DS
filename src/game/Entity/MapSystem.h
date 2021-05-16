#pragma once

#include "../MathLib.h"
#include "../Assets.h"
#include <vector>

class EntityManager;
class Camera;

class MapSystem {
	struct MinimapData {
		std::vector<Rectangle16> dst;

		inline void clear() {
			dst.clear();
		}
		inline size_t size() const { return dst.size(); }
	};
private:

	Vector2Int16 mapSize;
	Image minimapTexture = { 0,0 };
	Image minimapTerrainTexture = { 0,0 };

	MinimapData minimapData;

	void GenerateMiniampTerrainTexture();
	void RenderMinimap();
public:

	void SetSize(Vector2Int16 size);

	void UpdateMap(EntityManager& em);

	void DrawMap( const Camera& camera);

	void DrawMinimap(Rectangle dst);
};