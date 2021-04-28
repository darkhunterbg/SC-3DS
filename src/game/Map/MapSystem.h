#pragma once

#include "MapDef.h"
#include "../MathLib.h"
#include <vector>
#include "../Assets.h"
#include "../Camera.h"
#include "../Span.h"


struct MapTile {
	Vector2Int position;
	Sprite sprite;
	static constexpr const int TileSize = 32;
};

class MapSystem {

private:
	std::vector<MapTile> tiles;
	Rectangle mapBounds;
public:
	MapSystem(const MapDef& map);
	~MapSystem();

	Rectangle GetMapBounds() const { return mapBounds; }

	void DrawTiles(const Camera& camera);
	void DrawMiniMapTiles(Vector2Int offset, const Camera& camera);

	inline Span<MapTile> GetTiles() const {
		return { tiles.data(),tiles.size() };
	}
};