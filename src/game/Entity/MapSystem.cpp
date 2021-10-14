#include "MapSystem.h"

#include "../Engine/GraphicsRenderer.h"
#include "../Engine/AssetLoader.h"
#include "../Camera.h"
#include "EntityManager.h"

static ImageFrame _tile;

void MapSystem::DeleteEntities(std::vector<EntityId>& entities)
{
}

size_t MapSystem::ReportMemoryUsage()
{
	return size_t();
}

void MapSystem::RedrawMinimapFogOfWar(const PlayerVision& vision)
{
	if (_minimapFowTexture.surfaceId == nullptr)
	{
		_minimapFowTexture = GraphicsRenderer::NewRenderSurface({ _minimapTextureSize,_minimapTextureSize });
	}

	int mapSizeTiles = (int)_mapSize.x / 32;
	int multiplier = _minimapTextureSize / mapSizeTiles;

	GraphicsRenderer::DrawOnSurface(_minimapFowTexture);
	GraphicsRenderer::ClearCurrentSurface(Colors::Black);


	Color colors[2] = { (Color(0,0,0,0.5)), Color(0,0,0,0.0f) };

	GraphicsRenderer::ChangeBlendingMode(BlendMode::AllSet);

	for (short y = 0; y < mapSizeTiles; ++y)
	{
		for (short x = 0; x < mapSizeTiles; ++x)
		{

			uint8_t state = vision.GetState({ x,y });
			if (state)
			{
				const Color& c = colors[state - 1];
				GraphicsRenderer::DrawRectangle({ { x * multiplier, y * multiplier }, { multiplier, multiplier} }, c);
			}
		}
	}

	GraphicsRenderer::ChangeBlendingMode(BlendMode::Alpha);
}

void MapSystem::SetSize(Vector2Int16 size)
{
	_mapSize = size;
	_gridSize = size / 32;
	_minimapTextureSize = size.x / 32;

	_tile = GraphicsRenderer::NewSprite(*AssetLoader::LoadTexture("tileset\\tile"), { {0,0},{192,192} });
}

void MapSystem::DrawMap(const Camera& camera)
{
	Rectangle16 camRect = camera.GetRectangle16();

	static constexpr const short tileSize = 32 * 6;

	Vector2Int16 start = (camRect.position / tileSize) * tileSize;
	Vector2Int16 end = (camRect.GetMax() / tileSize + Vector2Int16{ 1, 1 }) * tileSize;

	for (short y = start.y; y < end.y; y += tileSize)
	{
		for (short x = start.x; x < end.x; x += tileSize)
		{
			Rectangle dst = { {x,y}, {tileSize, tileSize} };
			dst.position -= Vector2Int(camRect.position);
			dst.position /= camera.Scale;
			dst.size /= camera.Scale;

			GraphicsRenderer::Draw(_tile, dst);
		}
	}
}
void MapSystem::DrawFogOfWar(EntityManager& em, const Camera& camera)
{
	if (!em.PlayerSystem.HasPlayer(ActivePlayer))
		return;

	const PlayerVision& vision = em.PlayerSystem.GetPlayerVision(ActivePlayer);

	RedrawMinimapFogOfWar(vision);

	if (!FogOfWarVisible || _minimapFowTexture.surfaceId == nullptr)
		return;

	static constexpr const int Upscale = 4;

	Vector2Int16 FogOfWarTextureSize = Vector2Int16(_minimapTextureSize, _minimapTextureSize);
	FogOfWarTextureSize *= Upscale;


	GraphicsRenderer::ChangeBlendingMode(BlendMode::AllSet);

	if (_fogOfWarTexture.surfaceId == nullptr)
	{
		_fogOfWarTexture = GraphicsRenderer::NewRenderSurface(Vector2Int(FogOfWarTextureSize));
		_fowDownscaleTexture = GraphicsRenderer::NewRenderSurface(Vector2Int(_minimapTextureSize / 2));
	}

	Rectangle16 camRect = camera.GetRectangle16();


	GraphicsRenderer::DrawOnSurface(_fowDownscaleTexture);
	GraphicsRenderer::ClearCurrentSurface(Colors::Transparent);
	// downscale minimap fow
	GraphicsRenderer::Draw(_minimapFowTexture, _fowDownscaleTexture.GetRect());


	GraphicsRenderer::DrawOnSurface(_fogOfWarTexture);
	GraphicsRenderer::ClearCurrentSurface(Colors::Transparent);
	// upscale downscaled minimap fow
	GraphicsRenderer::Draw(_fowDownscaleTexture, _fogOfWarTexture.GetRect());


	GraphicsRenderer::DrawOnCurrentScreen();
	GraphicsRenderer::ChangeBlendingMode(BlendMode::Alpha);
	static constexpr const int CamDownscale = 32 / Upscale;
	Rectangle16 src = { (camRect.position / CamDownscale), (camRect.size / CamDownscale) };

	ImageFrame sprite = GraphicsRenderer::NewSprite(*_fogOfWarTexture.sprite.texture, src);

	GraphicsRenderer::Draw(sprite, { {0,0}, Vector2Int(camera.Size) });

	GraphicsRenderer::Submit();
}