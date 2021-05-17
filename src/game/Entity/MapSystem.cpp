#include "MapSystem.h"
#include "EntityManager.h"
#include "../Game.h"
#include "../Platform.h"

static Sprite tile;
static Sprite grad0h, grad0v, grad1, circle;

static constexpr const int MinimapTextureSize = 256;

void MapSystem::SetSize(Vector2Int16 size)
{
	mapSize = size;

	tile = Game::AssetLoader.LoadAtlas("tileset_tile.t3x")->GetSprite(0);
	//circle = Game::AssetLoader.LoadAtlas("other.t3x")->GetSprite(0);
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

	int div = 1;

	Vector2Int16 FogOfWarTextureSize = { 512,256 };
	FogOfWarTextureSize /= div;

	if (fogOfWarTexture.textureId == nullptr) {
		fogOfWarTexture = Platform::NewTexture(Vector2Int(FogOfWarTextureSize));
		fowSprite = Platform::NewSprite(fogOfWarTexture, { {0,0}, {400,240} });
	}

	Platform::DrawOnTexture(fogOfWarTexture.textureId);
	Platform::ClearBuffer(Colors::Black);

	Platform::ToggleAlphaOverride(true);
	//Platform::ToggleTestBlend();

	Rectangle16 camRect = camera.GetRectangle16();

	Vector2Int16 start = (camRect.position / 32);
	Vector2Int16 end = (camRect.GetMax() / 32) + Vector2Int16{ 1, 1 };
	const PlayerVision& v = *vision;


	for (short y = start.y; y < end.y; ++y) {
		for (short x = start.x; x < end.x; ++x) {

			Vector2Int16 t = { x,y };

			bool visible = false;

			for (const auto& c : v.visible) {
				if (c.Contains(t)) {
					visible = true;
					break;
				}
			}

			bool isKnown = v.IsKnown(t);

			float a = visible ? 0 : 0.66f;

			if (visible || isKnown) {
			/*	int p = 0;
				p += !v.IsKnown(t + Vector2Int16(-1, -1));
				p += !v.IsKnown(t + Vector2Int16(1, -1));
				p += !v.IsKnown(t + Vector2Int16(-1, 0));
				p += !v.IsKnown(t + Vector2Int16(1, 0));

				p += !v.IsKnown(t + Vector2Int16(-1, 1));
				p += !v.IsKnown(t + Vector2Int16(1, 1));
				p += !v.IsKnown(t + Vector2Int16(0, -1));
				p += !v.IsKnown(t + Vector2Int16(0, 1));*/

				Color c = Colors::Transparent;
				c.a = a;

				/*if (v.IsKnown(t))
					c.a = 0.5f;*/



				Rectangle dst = { {x * 32,y * 32}, {32, 32} };
				dst.position -= Vector2Int(camRect.position);
				dst.position /= camera.Scale;
				dst.size /= camera.Scale;
				dst.position /= div;
				dst.size /= div;

				//if(p==0)
				Platform::DrawRectangle(dst, c);

				//if (p >0) {
				//	//c = Colors::White;
				//	Vector2Int cntr = dst.GetCenter();
				//	//dst.position += dst.size / 2;
				//	dst.size *= 2;
				//	dst.SetCenter(cntr);

				//	Platform::Draw(circle, dst, c);



				//}

			}
		}
	}



	Platform::ToggleAlphaOverride(false);
	

	Platform::DrawOnTexture(nullptr);


	//fowSprite.image = fogOfWarTexture;
	//fowSprite.rect = { {0,0}, {400,240} };

	Platform::Draw(fowSprite, { {0,0},{400,240} });

	//Rectangle16 src = { (camRect.position / 8), (camRect.size / 8) };

	//static Sprite s = Platform::NewSprite(minimapVisionTexture, src);
	//s.image = minimapVisionTexture;
	//s.rect = src;

	//Platform::Draw(s, { {0,0},{400,240} });

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

	if (minimapVisionTexture.textureId == nullptr) {
		minimapVisionTexture = Platform::NewTexture({ MinimapTextureSize,MinimapTextureSize });
	}

	int mapSizeTiles = (int)mapSize.x / 32;
	int multiplier = MinimapTextureSize / mapSizeTiles;

	Platform::DrawOnTexture(minimapVisionTexture.textureId);
	Platform::ClearBuffer(Colors::Black);

	if (vision == nullptr)
		return;

	Color c = Colors::Transparent;
	Color sc = Colors::Transparent;
	sc.a = 0.6f;
	Platform::ToggleAlphaOverride(true);


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

	Platform::ToggleAlphaOverride(false);
}
void MapSystem::RenderMinimap() {
	if (minimapTexture.textureId == nullptr) {
		minimapTexture = Platform::NewTexture({ MinimapTextureSize,MinimapTextureSize });
	}

	int mapSizeTiles = (int)mapSize.x / 32;
	int multiplier = MinimapTextureSize / mapSizeTiles;


	RenderMinimapVision();

	Platform::DrawOnTexture(minimapTexture.textureId);
	//Platform::ClearBuffer(Colors::Black);

	Sprite fullMapSprite;
	fullMapSprite.rect = { {0,0},{MinimapTextureSize,MinimapTextureSize} };
	fullMapSprite.image = minimapTerrainTexture;
	Platform::Draw(fullMapSprite, { {0,0}, {MinimapTextureSize,MinimapTextureSize} });

	if (FogOfWarVisible) {
		fullMapSprite.image = minimapVisionTexture;
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
	minimapSprite.rect = { {0,0},{MinimapTextureSize,MinimapTextureSize} };
	minimapSprite.image = minimapTexture;

	Platform::Draw(minimapSprite, dst);

}
