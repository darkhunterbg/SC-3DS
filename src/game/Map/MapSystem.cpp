#include "MapSystem.h"
#include "../Platform.h"
#include "../Game.h"

static const SpriteAtlas* minerals;


MapSystem::MapSystem(const MapDef& map) {
	auto tile = Game::AssetLoader.LoadAtlas("tileset_tile.t3x")->GetSprite(0);
	minerals = Game::AssetLoader.LoadAtlas("unit_neutral_min01.t3x");

	Vector2Int16 size = { 6,6 };

	for (short i = 0; i < map.size.x; i += size.x) {
		for (short j = 0; j < map.size.y; j += size.y) {
			MapTile t;

			t.position = { i,j };
			t.sprite = tile;

			tiles.push_back(t);
		}
	}

	mapBounds.position = { 0,0 };
	mapBounds.size = map.size * Vector2Int16(32, 32);


}
MapSystem::~MapSystem() {

}


void MapSystem::DrawTiles(const Camera& camera) {
	Rectangle camRect = camera.GetRectangle();

	for (const MapTile& tile : tiles) {
		Rectangle dst = { Vector2Int( tile.position * Vector2Int16(32,32) ), Vector2Int( tile.sprite.rect.size )};

		if (!camRect.Intersects(dst))
			continue;

		dst.position -= camRect.position;
		dst.position /= camera.Scale;
		dst.size /= camera.Scale;


		Platform::Draw(tile.sprite, dst);
	}

	auto t = minerals->GetSprite(0);
	Rectangle dst = { {160,120}, Vector2Int( t.rect.size )};

	if (camRect.Intersects(dst)) {
		dst.position -= camRect.position;
		dst.position /= camera.Scale;
		dst.size /= camera.Scale;

		Platform::Draw(t, dst);
	}
}

void MapSystem::DrawMiniMapTiles(Vector2Int offset, const Camera& camera) {
	auto t = tiles[0].sprite;



	Vector2 downScale = Vector2(mapBounds.size) / Vector2(128, 128);

	if (downScale.x == 0)
		downScale.x = 1;
	if (downScale.y == 0)
		downScale.y = 1;

	Vector2Int size = Vector2Int(Vector2(minerals->GetSprite(0).rect.size) / downScale);
	Vector2Int pos = Vector2Int(Vector2(160, 120) / downScale);
	pos += offset;

	Platform::DrawRectangle({ pos,size }, Colors::LightBlue);
}