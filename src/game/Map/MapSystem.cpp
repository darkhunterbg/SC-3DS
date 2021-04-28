#include "MapSystem.h"
#include "../Platform.h"

static const SpriteAtlas* minerals;


MapSystem::MapSystem(const MapDef& map) {
	auto tile = Platform::LoadAtlas("tileset_tiles.t3x")->GetSprite(0);
	minerals = Platform::LoadAtlas("unit_neutral_min01.t3x");

	Vector2Int size = { 6,6 };

	for (int i = 0; i < map.size.x; i += size.x) {
		for (int j = 0; j < map.size.y; j += size.y) {
			MapTile t;

			t.position = { i,j };
			t.sprite = tile;

			tiles.push_back(t);
		}
	}

	mapBounds.position = { 0,0 };
	mapBounds.size = map.size * Vector2Int(32, 32);


}
MapSystem::~MapSystem() {

}


void MapSystem::DrawTiles(const Camera& camera) {
	Rectangle camRect = camera.GetRectangle();

	for (const MapTile& tile : tiles) {
		Rectangle dst = { tile.position * Vector2Int(32,32) , tile.sprite.rect.size };

		if (!camRect.Intersects(dst))
			continue;

		dst.position -= camRect.position;
		dst.position /= camera.Scale;
		dst.size /= camera.Scale;


		Platform::Draw(tile.sprite, dst, Colors::White);
	}

	auto t = minerals->GetSprite(0);
	Rectangle dst = { {160,120}, t.rect.size };

	if (camRect.Intersects(dst)) {
		dst.position -= camRect.position;
		dst.position /= camera.Scale;
		dst.size /= camera.Scale;

		Platform::Draw(t, dst, Colors::White);
	}
}

void MapSystem::DrawMiniMapTiles(Vector2Int offset, const Camera& camera) {
	auto t = tiles[0].sprite;



	Vector2 downScale = Vector2(mapBounds.size) / Vector2(128, 128);

	if (downScale.x == 0)
		downScale.x = 1;
	if (downScale.y == 0)
		downScale.y = 1;

	Vector2Int size = minerals->GetSprite(0).rect.size / downScale;
	Vector2Int pos = Vector2Int{ 160,120 } / downScale;
	pos += offset;

	Platform::DrawRectangle({ pos,size }, Colors::LightBlue);
}