#include "MapSystem.h"

#include "../Engine/GraphicsRenderer.h"
#include "../Engine/AssetLoader.h"
#include "../Camera.h"
#include "EntityManager.h"
#include "../Profiler.h"

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
	if (!FogOfWarVisible) return;

	if (_minimapFowTexture.surfaceId == nullptr)
	{
		_minimapFowTexture = GraphicsRenderer::NewRenderSurface({ _minimapTextureSize,_minimapTextureSize });
	}

	int mapSizeTiles = (int)_mapSize.x / 32;
	int multiplier = _minimapTextureSize / mapSizeTiles;

	GraphicsRenderer::DrawOnSurface(_minimapFowTexture);
	GraphicsRenderer::ClearCurrentSurface(Colors::Black);

	_halfLitTiles.clear();
	_fullLitTiles.clear();

	Color colors[2] = { (Color(0,0,0,0.5)), Color(0,0,0,0.0f) };

	std::vector<Rectangle16>* renderTiles[] = { &_halfLitTiles, &_fullLitTiles };

	GraphicsRenderer::ChangeBlendingMode(BlendMode::AllSet);

	for (short y = 0; y < mapSizeTiles; ++y)
	{
		Rectangle16 start = { {0,y} ,{1,1} };
		uint8_t currentState = vision.GetState(start.position);

		for (short x = 1; x < mapSizeTiles; ++x)
		{
			uint8_t state = vision.GetState({ x,y });
			if (state != currentState)
			{
				if (currentState)
				{
					Rectangle16 add = start;
					add.size.x = x - add.position.x + 1;
					renderTiles[currentState - 1]->push_back(add);
				}

				currentState = state;
				start = { {x,y }, { 1,1 } };

			}
		}


		Rectangle16 add = start;
		uint8_t state = vision.GetState({ (short)(mapSizeTiles - 1),y });
		if (state)
		{
			add.size.x = mapSizeTiles - 1 - add.position.x + 1;
			renderTiles[state - 1]->push_back(add);
		}

	}

	for (auto& rect : _halfLitTiles)
	{
		const Color& c = colors[0];
		GraphicsRenderer::DrawRectangle({ Vector2Int(rect.position * multiplier), Vector2Int(rect.size * multiplier) }, c);
	}

	for (auto& rect : _fullLitTiles)
	{
		const Color& c = colors[1];
		GraphicsRenderer::DrawRectangle({ Vector2Int(rect.position * multiplier), Vector2Int(rect.size * multiplier) }, c);
	}

	GraphicsRenderer::ChangeBlendingMode(BlendMode::Alpha);

	GraphicsRenderer::DrawOnCurrentScreen();
}

void MapSystem::SetSize(Vector2Int16 size)
{
	_mapSize = size;
	_gridSize = size / 32;
	_minimapTextureSize = size.x / 32;

	_tile = GraphicsRenderer::NewSprite(*AssetLoader::LoadTexture("tileset\\tile"), { {0,0},{192,192} });
}

void MapSystem::DrawOffscreenData(EntityManager& em)
{
	if (!em.PlayerSystem.HasPlayer(ActivePlayer))
		return;

	const PlayerVision& vision = em.PlayerSystem.GetPlayerVision(ActivePlayer);

	RedrawMinimapFogOfWar(vision);
}

void MapSystem::UpdateVisibleObjects(EntityManager& em)
{
	if (!em.PlayerSystem.HasPlayer(ActivePlayer))
		return;

	const PlayerVision& vision = em.PlayerSystem.GetPlayerVision(ActivePlayer);

	for (EntityId id : em.UnitSystem.GetEntities())
	{
		DrawComponent& draw = em.DrawSystem.GetComponent(id);
		auto bb = draw.boundingBox;
		bb.Restrict({ 0,0 }, _mapSize);
		bb.position = bb.position >> 5;
		bb.size = bb.size >> 5;
		draw.visible = !FogOfWarVisible || vision.IsVisible(bb);
	}
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
	if (!FogOfWarVisible || _minimapFowTexture.surfaceId == nullptr)
		return;

	static constexpr const int Upscale = 4;

	Vector2Int16 FogOfWarTextureSize = Vector2Int16(_minimapTextureSize, _minimapTextureSize);
	FogOfWarTextureSize *= Upscale;

	GraphicsRenderer::ChangeBlendingMode(BlendMode::AllSet);

	if (_fogOfWarTexture.surfaceId == nullptr)
	{
#ifdef _3DS
		if (FogOfWarTextureSize.x == 1024)
			FogOfWarTextureSize /= 2;
#endif


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