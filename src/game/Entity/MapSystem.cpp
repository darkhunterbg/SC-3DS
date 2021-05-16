#include "MapSystem.h"
#include "EntityManager.h"
#include "../Game.h"
#include "../Platform.h"

static Sprite tile;

static constexpr const int MinimapTextureSize = 256;

void MapSystem::SetSize(Vector2Int16 size)
{
	mapSize = size;

	tile = Game::AssetLoader.LoadAtlas("tileset_tile.t3x")->GetSprite(0);
}

void MapSystem::UpdateMap(EntityManager& em)
{
	minimapData.clear();

	for (EntityId id : em.UnitArchetype.Archetype.GetEntities()) {
		if (em.FlagComponents.GetComponent(id).test(ComponentFlags::RenderEnabled))
		{
			auto collider = em.CollisionArchetype.ColliderComponents.GetComponent(id).collider;
			collider.position += em.PositionComponents.GetComponent(id);
			minimapData.dst.push_back(collider);
		}
	}
}

void MapSystem::DrawMap(const Camera& camera)
{
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


void MapSystem::GenerateMiniampTerrainTexture() {
	Rectangle mapBounds = { {0,0}, Vector2Int(mapSize) };

	minimapTerrainTexture = Platform::NewTexture({ MinimapTextureSize,MinimapTextureSize });
	Platform::DrawOnTexture(minimapTerrainTexture.textureId);

	Vector2 upscale = Vector2(mapBounds.size) / Vector2(MinimapTextureSize, MinimapTextureSize);

	Vector2Int tileSize = { 32 * 6  , 32 * 6 };

	for (int y = 0; y < mapSize.y; y += tileSize.y) {
		for (int x = 0; x < mapSize.x; x += tileSize.x) {
			Rectangle dst = { {x,y},{mapSize.x, mapSize.y} };
			dst.position = Vector2Int(Vector2(x, y) / upscale);
			dst.size = Vector2Int(Vector2(tileSize.x, tileSize.y) / upscale);
			Platform::Draw(tile, dst);
		}
	}

	Platform::DrawOnTexture(nullptr);
}
void MapSystem::RenderMinimap() {
	if (minimapTexture.textureId == nullptr) {
		minimapTexture = Platform::NewTexture({ MinimapTextureSize,MinimapTextureSize });
	}

	Platform::DrawOnTexture(minimapTexture.textureId);

	Sprite terrainSprite;
	terrainSprite.rect = { {0,0},{MinimapTextureSize,MinimapTextureSize} };
	terrainSprite.image = minimapTerrainTexture;

	Platform::Draw(terrainSprite, { {0,0}, {MinimapTextureSize,MinimapTextureSize} });

	int mapSizeTiles = (int)mapSize.x / 32;
	int multiplier = MinimapTextureSize / mapSizeTiles;


	//Platform::DrawRectangle({ {16,4},{4,4} }, Colors::MapFriendly);
	for (const Rectangle16& collider : minimapData.dst) {
		Rectangle dst;
		dst.position = (Vector2Int(collider.GetCenter()) * multiplier) / 32;
		dst.size = (Vector2Int(collider.size) * multiplier) / 16;
		if (dst.size.x < multiplier * 2)
			dst.size.x = multiplier * 2;
		if (dst.size.y < multiplier * 2)
			dst.size.y = multiplier * 2;

		Platform::DrawRectangle(dst, Colors::MapFriendly);
	}

	Platform::DrawOnTexture(nullptr);
}

void MapSystem::DrawMinimap(Rectangle dst)
{
	if (minimapTerrainTexture.textureId == nullptr) {
		GenerateMiniampTerrainTexture();
	}

	RenderMinimap();

	Sprite minimapSprite;
	minimapSprite.rect = { {0,0},{MinimapTextureSize,MinimapTextureSize} };
	minimapSprite.image = minimapTexture;

	Platform::Draw(minimapSprite, dst);

}
