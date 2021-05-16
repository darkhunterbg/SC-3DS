#include "MapSystem.h"
#include "EntityManager.h"
#include "../Game.h"
#include "../Platform.h"

static Sprite tile;

static constexpr const int MinimapTextureSize = 256;

MapSystem::MapSystem()
{
	minimapSprite.image = { 0,0 };
}

void MapSystem::SetSize(Vector2Int16 size)
{
	mapSize = size;

	tile = Game::AssetLoader.LoadAtlas("tileset_tile.t3x")->GetSprite(0);
}

void MapSystem::UpdateMap(EntityManager& em)
{
}

void MapSystem::DrawMap(const Camera& camera)
{
	DrawMinimap();

	Rectangle16 camRect = camera.GetRectangle16();

	short tileSize = 32 * 6;

	Vector2Int16 start = (camRect.position / tileSize) * tileSize;
	Vector2Int16 end = (camRect.GetMax() / tileSize + Vector2Int16{ 1, 1 }) * tileSize;

	for (short y = start.y; y < end.y; y += tileSize) {
		for (short x = start.x; x < end.x; x += tileSize) {
			Rectangle dst = { {x,y}, {tileSize, tileSize} };
			dst.position -= Vector2Int(camRect.position);
			dst.position /= camera.Scale;
			dst.size /= camera.Scale;

			Platform::Draw(tile, dst);
		}
	}
}

void MapSystem::RenderMinimapTexture() {
	Rectangle mapBounds = { {0,0}, Vector2Int(mapSize) };

	minimapTexture = Platform::NewTexture({ MinimapTextureSize,MinimapTextureSize });
	Platform::DrawOnTexture(minimapTexture.textureId);

	Vector2 upscale = Vector2(mapBounds.size) / Vector2(MinimapTextureSize, MinimapTextureSize);

	Vector2Int tileSize = { 32 * 6  , 32 * 6 };

	for (int y = 0; y < mapSize.y; y += tileSize.y) {
		for (int x = 0; x < mapSize.x; x += tileSize.x) {
			Rectangle dst = { {x,y},{mapSize.x, mapSize.y} };
			dst.position = Vector2Int(Vector2( x,y ) / upscale);
			dst.size = Vector2Int(Vector2(tileSize.x, tileSize.y) / upscale);
			Platform::Draw(tile, dst);
		}
	}

	Platform::DrawOnTexture(nullptr);
	minimapSprite.image = minimapTexture;
	minimapSprite.rect = { {0,0},{MinimapTextureSize,MinimapTextureSize} };
}

void MapSystem::DrawMinimap()
{
	if (minimapTexture.textureId == nullptr) {
		RenderMinimapTexture();
	}
}
