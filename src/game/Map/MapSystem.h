#pragma once

#include "MapDef.h"
#include "../MathLib.h"
#include <vector>
#include "../Assets.h"
#include "../Camera.h"

class MapSystem {

	struct Tile {
		Vector2Int position;
		Sprite sprite;
	};

private:
	std::vector<Tile> tiles;
	Rectangle mapBounds;
public:
	MapSystem(const MapDef& map);
	~MapSystem();

	Rectangle GetMapBounds() const { return mapBounds; }

	void DrawTiles(const Camera& camera);
	void DrawMiniMapTiles(Vector2Int offset, const Camera& camera);
};