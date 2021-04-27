#include "MapSystem.h"
#include "../Platform.h"

static const SpriteAtlas* minerals;
Texture minimapTexture = nullptr;

MapSystem::MapSystem(const MapDef& map) {
	auto tile = Platform::LoadAtlas("tileset_tiles.t3x")->GetSprite(0);
	minerals = Platform::LoadAtlas("unit_neutral_min01.t3x");

	Vector2Int size = { 6,6 };

	for (int i = 0; i < map.size.x; i += size.x) {
		for (int j = 0; j < map.size.y; j += size.y) {
			Tile t;

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
	if (minimapTexture == nullptr) {
		minimapTexture = Platform::NewTexture({ 256,256 });
		Platform::DrawOnTexture(minimapTexture);

		Vector2Int downScale = (mapBounds.size) / Vector2Int(256, 256);
		if (downScale.x == 0)
			downScale.x = 1;
		if (downScale.y == 0)
			downScale.y = 1;

		for (const Tile& tile : tiles) {
			Vector2Int pos = tile.position * Vector2Int(32, 32);
			Rectangle dst = { pos / downScale , tile.sprite.rect.size / downScale };
			Platform::Draw(tile.sprite, dst, Colors::White);
		}

		Platform::DrawOnTexture(nullptr);
	}


	Rectangle camRect = camera.GetRectangle();

	for (const Tile& tile : tiles) {
		Rectangle dst = { tile.position * Vector2Int(32,32) , tile.sprite.rect.size };

		if (!camRect.Intersects(dst))
			continue;

		dst.position -= camRect.position;
		dst.position /= camera.Scale;
		dst.size /= camera.Scale;

		//dst.position /= camera.Scale;

		//dst.position += (camera.Size - dst.size) / 2;


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


	Sprite minimapSprite;
	minimapSprite.textureId = minimapTexture;
	minimapSprite.rect = { {0,0},{256,256} };

	Platform::Draw(minimapSprite, { offset,{128,128} }, Colors::White);

	Vector2 downScale = Vector2(mapBounds.size) / Vector2(128, 128);

	if (downScale.x == 0)
		downScale.x = 1;
	if (downScale.y == 0)
		downScale.y = 1;

	Vector2Int size = minerals->GetSprite(0).rect.size / downScale;
	Vector2Int pos = Vector2Int{ 160,120 } / downScale;
	pos += offset;

	Platform::DrawRectangle({ pos,size }, Colors::LightBlue);



	Rectangle camRect = camera.GetRectangle();
	//camRect.position += camRect.size / 2;
	Vector2 min = camRect.GetMin();
	min /= downScale;
	Vector2 max = camRect.GetMax();
	max /= downScale;
	min += offset;
	max += offset;
	Vector2 s = Vector2(camRect.size) / downScale;


	Platform::DrawLine(min, min + Vector2(s.x, 0), Colors::White);
	Platform::DrawLine(max, max - Vector2(s.x, 0), Colors::White);
	Platform::DrawLine(min, min + Vector2(0, s.y), Colors::White);
	Platform::DrawLine(max, max - Vector2(0, s.y), Colors::White);
}