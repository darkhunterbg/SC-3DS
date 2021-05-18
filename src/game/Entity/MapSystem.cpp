#include "MapSystem.h"
#include "EntityManager.h"
#include "../Game.h"
#include "../Platform.h"

static Sprite tile;



enum class EdgeType : uint8_t {
	None = 0b0000,
	Full = 0b1111,

	Left1 = 0b0001,
	Left3 = 0b1011,
	UpLeft2 = 0b0011,
	Up1 = 0b0010,
	Up3 = 0b0111,
	UpRight2 = 0b0110,
	Right1 = 0b0100,
	Right3 = 0b1110,
	DownRight2 = 0b1100,
	Down1 = 0b1000,
	Down3 = 0b1101,
	DownLeft2 = 0b1001,

};

void MapSystem::SetSize(Vector2Int16 size)
{
	mapSize = size;

	MinimapTextureSize = size.x / 32;

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

	vision = &em.GetPlayerSystem().GetPlayerVision(0);
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


static Sprite fowSprite;

void MapSystem::DrawFogOfWar(const Camera& camera) {
	if (!FogOfWarVisible || vision == nullptr)
		return;


	Vector2Int16 FogOfWarTextureSize = Vector2Int16(MinimapTextureSize, MinimapTextureSize);
	FogOfWarTextureSize *= 4;

	static Sprite fowDownscaleSprite;
	Platform::ChangeBlendingMode(BlendMode::FullOverride);

	if (fogOfWarTexture.textureId == nullptr) {
		fogOfWarTexture = Platform::NewTexture(Vector2Int(FogOfWarTextureSize));
		Image t = Platform::NewTexture(Vector2Int(MinimapTextureSize / 2));
		fowDownscaleSprite = Platform::NewSprite(t, { {0,0},Vector2Int16(MinimapTextureSize / 2) });
	}

	Rectangle16 camRect = camera.GetRectangle16();

	Platform::DrawOnTexture(fowDownscaleSprite.image.textureId);
	Platform::ClearBuffer(Colors::Transparent);

	// downscale minimap fow
	Platform::Draw({ { {0,0}, {MinimapTextureSize, MinimapTextureSize}}, minimapFowTexture },
		{ {0,0},{fowDownscaleSprite.rect.size.x,fowDownscaleSprite.rect.size.y} });

	Platform::DrawOnTexture(fogOfWarTexture.textureId);
	Platform::ClearBuffer(Colors::Transparent);

	// upscale downscaled minimap fow
	Platform::Draw(fowDownscaleSprite,
		{ {0,0},{FogOfWarTextureSize.x,FogOfWarTextureSize.y} });

	Platform::DrawOnTexture(nullptr);
	Platform::ChangeBlendingMode(BlendMode::Alpha);

	Rectangle16 src =  { (camRect.position / 8), (camRect.size / 8) };
	Sprite fowSprite = Platform::NewSprite(fogOfWarTexture, src);


	Platform::Draw(fowSprite, { {0,0},{400,240} });
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

void MapSystem::RenderMinimapVision() {

	if (minimapFowTexture.textureId == nullptr) {
		minimapFowTexture = Platform::NewTexture({ MinimapTextureSize,MinimapTextureSize });
	}

	int mapSizeTiles = (int)mapSize.x / 32;
	int multiplier = MinimapTextureSize / mapSizeTiles;

	Platform::DrawOnTexture(minimapFowTexture.textureId);
	Platform::ClearBuffer(Colors::Black);

	if (vision == nullptr)
		return;

	Color c = Colors::Transparent;
	Color sc = Colors::Transparent;
	sc.a = 0.6f;
	Platform::ChangeBlendingMode(BlendMode::FullOverride);


	for (short y = 0; y < mapSizeTiles; ++y) {
		for (short x = 0; x < mapSizeTiles; ++x) {
			if (vision->IsKnown({ x,y })) {
				Rectangle dst = { {x * multiplier, y * multiplier},{ multiplier, multiplier} };
				Platform::DrawRectangle(dst, sc);
			}
		}
	}

	for (const Circle16& circle : vision->visible) {
		Vector2Int16 min = circle.position - Vector2Int16(circle.size, circle.size);
		Vector2Int16 max = circle.position + Vector2Int16(circle.size, circle.size);
		for (short y = min.y; y < max.y; ++y) {
			for (short x = min.x; x < max.x; ++x) {
				// TODO: mapSizeTiles should be vector
				if (x < 0 || y < 0 || x >= mapSizeTiles || y >= mapSizeTiles)
					continue;

				if (circle.Contains(Vector2Int16(x, y))) {
					Rectangle dst = { {x * multiplier, y * multiplier},{ multiplier, multiplier} };
					Platform::DrawRectangle(dst, c);
				}
			}
		}
	}

	Platform::ChangeBlendingMode(BlendMode::Alpha);
}
void MapSystem::RenderMinimap() {
	if (minimapTexture.textureId == nullptr) {
		minimapTexture = Platform::NewTexture({ MinimapTextureSize,MinimapTextureSize }, true);
	}

	int mapSizeTiles = (int)mapSize.x / 32;
	int multiplier = MinimapTextureSize / mapSizeTiles;


	RenderMinimapVision();

	Platform::DrawOnTexture(minimapTexture.textureId);
	//Platform::ClearBuffer(Colors::Black);

	Sprite fullMapSprite;
	fullMapSprite.rect = { {0,0}, Vector2Int16(MinimapTextureSize,MinimapTextureSize) };
	fullMapSprite.image = minimapTerrainTexture;
	Platform::Draw(fullMapSprite, { {0,0}, {MinimapTextureSize,MinimapTextureSize} });

	if (FogOfWarVisible) {
		fullMapSprite.image = minimapFowTexture;
		Platform::Draw(fullMapSprite, { {0,0}, {MinimapTextureSize,MinimapTextureSize} });
	}

	//Platform::DrawRectangle({ {16,4},{4,4} }, Colors::MapFriendly);
	for (const Rectangle16& collider : minimapData.dst) {
		Rectangle dst;
		dst.position = (Vector2Int(collider.position) / 32) * multiplier;
		dst.size = (Vector2Int(collider.size) / 16) * multiplier;
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
	minimapSprite.rect = { {0,0}, Vector2Int16(MinimapTextureSize,MinimapTextureSize) };
	minimapSprite.image = minimapTexture;

	Platform::Draw(minimapSprite, dst);

}
