#include "MapSystem.h"
#include "EntityManager.h"

#include "../Profiler.h"
#include "../Engine/GraphicsRenderer.h"
#include "../Engine/AssetLoader.h"

static ImageFrame tile;

void MapSystem::SetSize(Vector2Int16 size)
{
	mapSize = size;
	gridSize = size / 32;
	minimapTextureSize = size.x / 32;

	tile = GraphicsRenderer::NewSprite(*AssetLoader::LoadTexture("tileset\\tile"), { {0,0},{192,192} });
}

void MapSystem::UpdateMapObjectPositions(EntityManager& em, const EntityChangedData& changed) {

	int end = changed.size();
	for (int i = 0; i < end; ++i) {
		EntityId id = changed.entity[i];
		const auto& bb = em.MapObjectArchetype.BoundingBoxComponents.GetComponent(id);
		auto& dst = em.MapObjectArchetype.DestinationComponents.GetComponent(id);
		dst = bb;
		dst.position += changed.position[i] >> 5;
		dst.Restrict(Vector2Int16(0, 0), gridSize - Vector2Int16(1, 1));
	}
}

void MapSystem::UpdateMap(EntityManager& em, const EntityChangedData& changed)
{
	UpdateMapObjectPositions(em, changed);

	minimapData.clear();

	const std::vector<PlayerInfo>& players = em.GetPlayerSystem().GetPlayers();

	Color32 colors[16];
	for (int i = 0; i < players.size(); ++i) {
		colors[i] = players[i].color;
	}
	colors[15] = Color32(Colors::MapResource);
	colors[ActivePlayer] = Color32(Colors::MapFriendly);
	const PlayerVision& vision = em.GetPlayerSystem().GetPlayerVision(ActivePlayer);

	for (EntityId id : em.MapObjectArchetype.Archetype.GetEntities()) {
		if (em.UnitArchetype.Archetype.HasEntity(id) && !em.HiddenArchetype.Archetype.HasEntity(id))
		{
			uint8_t colorId = em.MapObjectArchetype.MinimapColorId.GetComponent(id);
			const Rectangle16& dst = em.MapObjectArchetype.DestinationComponents.GetComponent(id);

			bool visible = vision.IsVisible(dst);

			if (!visible && FogOfWarVisible)
				continue;

			minimapData.dst.push_back(dst);
			minimapData.color.push_back(colors[colorId]);
		}
	}

	for (EntityId id : em.UnitArchetype.FowVisibleArchetype.Archetype.GetEntities()) {

		PlayerId owner = em.UnitArchetype.OwnerComponents.GetComponent(id);
		const Rectangle16& dst = em.MapObjectArchetype.DestinationComponents.GetComponent(id);

		minimapData.dst.push_back(dst);
		minimapData.color.push_back(colors[owner]);
	}
}

void MapSystem::UpdateVisibleRenderEntities(EntityManager& em) {
	auto& vision = em.GetPlayerSystem().GetPlayerVision(ActivePlayer);

	removedEntities.clear();

	for (EntityId id : em.RenderArchetype.Archetype.GetEntities()) {
		if (!em.MapObjectArchetype.Archetype.HasEntity(id))
			continue;

		const Rectangle16& dst = em.MapObjectArchetype.DestinationComponents.GetComponent(id);
		bool visible = vision.IsVisible(dst);

		if (!visible && FogOfWarVisible) {
			removedEntities.push_back(id);
		}
	}

	addedEntities.clear();

	for (EntityId id : em.HiddenArchetype.Archetype.GetEntities()) {

		const Rectangle16& dst = em.MapObjectArchetype.DestinationComponents.GetComponent(id);
		bool visible = vision.IsVisible(dst);

		if (visible || !FogOfWarVisible) {
			addedEntities.push_back(id);
			em.FlagComponents.GetComponent(id).set(ComponentFlags::RenderChanged);
		}
	}

	em.RenderArchetype.Archetype.RemoveSortedEntities(removedEntities);
	em.HiddenArchetype.Archetype.AddSortedEntities(removedEntities);
	em.HiddenArchetype.Archetype.RemoveSortedEntities(addedEntities);
	em.RenderArchetype.Archetype.AddSortedEntities(addedEntities);
}
void MapSystem::UpdateVisibleRenderUnits(EntityManager& em) {

	const PlayerVision& vision = em.GetPlayerSystem().GetPlayerVision(ActivePlayer);

	removedEntities.clear();

	//for (EntityId id : em.UnitArchetype.RenderArchetype.Archetype.GetEntities()) {
	//	if (!em.MapObjectArchetype.Archetype.HasEntity(id))
	//		continue;

		// TODO: culling by MapObjectDestination is not very accurate for items that extend beyound shadows,
		// While culling by RenderBoundingBox is not very accurate when to reveal hidden units due proximity

		// Accurate culling: renderBB-> mapBB-> spriteBB
		// Or when unit becomes visible by renderBB, render only portion that is in los (slow)

	//	const Rectangle16& dst = em.MapObjectArchetype.DestinationComponents.GetComponent(id);
	//	bool visible = vision.IsVisible(dst);

	//	if (!visible && FogOfWarVisible) {
	//		removedEntities.push_back(id);
	//		em.FlagComponents.GetComponent(id).set(ComponentFlags::SoundMuted);
	//	}
	//}

	addedEntities.clear();

	for (EntityId id : em.UnitArchetype.HiddenArchetype.Archetype.GetEntities()) {

		const Rectangle16& dst = em.MapObjectArchetype.DestinationComponents.GetComponent(id);
		bool visible = vision.IsVisible(dst);

		if (visible || !FogOfWarVisible) {
			addedEntities.push_back(id);
			em.FlagComponents.GetComponent(id).set(ComponentFlags::RenderChanged);
			em.FlagComponents.GetComponent(id).clear(ComponentFlags::SoundMuted);
		}
	}

	//em.UnitArchetype.RenderArchetype.Archetype.RemoveSortedEntities(removedEntities);
	em.UnitArchetype.HiddenArchetype.Archetype.AddSortedEntities(removedEntities);
	em.UnitArchetype.HiddenArchetype.Archetype.RemoveSortedEntities(addedEntities);
	//em.UnitArchetype.RenderArchetype.Archetype.AddSortedEntities(addedEntities);
}

static MapSystem* s;
static EntityManager* e;

void MapSystem::InitFowVisibleEntitiesJob(int start, int end) {
	MapSystem& system = *s;
	EntityManager& em = *e;

	for (int i = start; i < end; ++i) {
		EntityId from = system.scratch2[i];
		EntityId id = system.scratch[i];

		em.PositionComponents.CopyComponent(from, id);

/*		em.UnitArchetype.RenderArchetype.RenderComponents.CopyComponent(from, id);
		em.UnitArchetype.RenderArchetype.BoundingBoxComponents.CopyComponent(from, id);
		em.UnitArchetype.RenderArchetype.OffsetComponents.CopyComponent(from, id);
		em.UnitArchetype.RenderArchetype.DestinationComponents.CopyComponent(from, id)*/;

		em.UnitArchetype.OwnerComponents.CopyComponent(from, id);

		em.MapObjectArchetype.BoundingBoxComponents.CopyComponent(from, id);
		em.MapObjectArchetype.DestinationComponents.CopyComponent(from, id);
		em.MapObjectArchetype.MinimapColorId.CopyComponent(from, id);

		em.FlagComponents.GetComponent(id).set(ComponentFlags::RenderEnabled);
	}
}

void MapSystem::UpdateFowVisibleUnits(EntityManager& em) {

	const PlayerVision& vision = em.GetPlayerSystem().GetPlayerVision(ActivePlayer);

	scratch.clear();

	for (EntityId id : em.UnitArchetype.FowVisibleArchetype.Archetype.GetEntities()) {

		const Rectangle16& dst = em.MapObjectArchetype.DestinationComponents.GetComponent(id);
		bool visible = vision.IsVisible(dst);

		if (visible || !FogOfWarVisible) {
			scratch.push_back(id);
		}
	}

	em.DeleteEntities(scratch);


	if (removedEntities.size() > 0)
	{

		auto& fowVisible = scratch2;
		fowVisible.clear();

		for (EntityId id : removedEntities) {
			// TODO: optimize at some point
			if (em.UnitArchetype.DataComponents.GetComponent(id).isBuilding) {
				fowVisible.push_back(id);
			}
		}

		if (fowVisible.size() > 0)
		{
			scratch.clear();
			em.NewEntities(fowVisible.size(), scratch);
			//em.UnitArchetype.RenderArchetype.Archetype.AddSortedEntities(scratch);
			em.UnitArchetype.FowVisibleArchetype.Archetype.AddSortedEntities(scratch);

			s = this;
			e = &em;

			JobSystem::RunJob(scratch.size(), JobSystem::DefaultJobSize, InitFowVisibleEntitiesJob);
		}
	}

}

void MapSystem::UpdateVisibleEntities(EntityManager& em) {
	UpdateVisibleRenderEntities(em);
	UpdateVisibleRenderUnits(em);
	UpdateFowVisibleUnits(em);
}

void MapSystem::DrawMap(const Camera& camera)
{
	Rectangle16 camRect = camera.GetRectangle16();

	static constexpr const short tileSize = 32 * 6;

	Vector2Int16 start = (camRect.position / tileSize) * tileSize;
	Vector2Int16 end = (camRect.GetMax() / tileSize + Vector2Int16{ 1, 1 }) * tileSize;

	for (short y = start.y; y < end.y; y += tileSize) {
		for (short x = start.x; x < end.x; x += tileSize) {
			Rectangle dst = { {x,y}, {tileSize, tileSize} };
			dst.position -= Vector2Int(camRect.position);
			dst.position /= camera.Scale;
			dst.size /= camera.Scale;

			GraphicsRenderer::Draw(tile, dst);
		}
	}
}

void MapSystem::DrawFogOfWar(const Camera& camera) {
	if (!FogOfWarVisible || minimapFowTexture.surfaceId == nullptr)
		return;

	static constexpr const int Upscale = 4;

	Vector2Int16 FogOfWarTextureSize = Vector2Int16(minimapTextureSize, minimapTextureSize);
	FogOfWarTextureSize *= Upscale;


	GraphicsRenderer::ChangeBlendingMode(BlendMode::AllSet);

	if (fogOfWarTexture.surfaceId == nullptr) {
		fogOfWarTexture = GraphicsRenderer::NewRenderSurface(Vector2Int(FogOfWarTextureSize));
		fowDownscaleTexture = GraphicsRenderer::NewRenderSurface(Vector2Int(minimapTextureSize / 2));
	}

	Rectangle16 camRect = camera.GetRectangle16();


	GraphicsRenderer::DrawOnSurface(fowDownscaleTexture);
	GraphicsRenderer::ClearCurrentSurface(Colors::Transparent);
	// downscale minimap fow
	GraphicsRenderer::Draw(minimapFowTexture, fowDownscaleTexture.GetRect());


	GraphicsRenderer::DrawOnSurface(fogOfWarTexture);
	GraphicsRenderer::ClearCurrentSurface(Colors::Transparent);
	// upscale downscaled minimap fow
	GraphicsRenderer::Draw(fowDownscaleTexture, fogOfWarTexture.GetRect());


	GraphicsRenderer::DrawOnCurrentScreen();
	GraphicsRenderer::ChangeBlendingMode(BlendMode::Alpha);
	static constexpr const int CamDownscale = 32 / Upscale;
	Rectangle16 src = { (camRect.position / CamDownscale), (camRect.size / CamDownscale) };

	ImageFrame sprite = GraphicsRenderer::NewSprite(*fogOfWarTexture.sprite.texture, src);

	GraphicsRenderer::Draw(sprite, { {0,0},{400,240} });

	GraphicsRenderer::Submit();
}

void MapSystem::GenerateMinimapTerrainTexture() {
	Rectangle mapBounds = { {0,0}, Vector2Int(mapSize) };

	minimapTerrainTexture = GraphicsRenderer::NewRenderSurface({ minimapTextureSize,minimapTextureSize });
	GraphicsRenderer::DrawOnSurface(minimapTerrainTexture);

	Vector2 upscale = Vector2(mapBounds.size) / Vector2(minimapTextureSize, minimapTextureSize);

	Vector2Int tileSize = { 32 * 6  , 32 * 6 };

	for (int y = 0; y < mapSize.y; y += tileSize.y) {
		for (int x = 0; x < mapSize.x; x += tileSize.x) {
			Rectangle dst = { {x,y},{mapSize.x, mapSize.y} };
			dst.position = Vector2Int(Vector2(x, y) / upscale);
			dst.size = Vector2Int(Vector2(tileSize.x, tileSize.y) / upscale);
			GraphicsRenderer::Draw(tile, dst);
		}
	}

}

void MapSystem::RedrawMinimapFogOfWar(const PlayerVision& vision) {

	if (minimapFowTexture.surfaceId == nullptr) {
		minimapFowTexture = GraphicsRenderer::NewRenderSurface({ minimapTextureSize,minimapTextureSize });
	}

	int mapSizeTiles = (int)mapSize.x / 32;
	int multiplier = minimapTextureSize / mapSizeTiles;

	GraphicsRenderer::DrawOnSurface(minimapFowTexture);
	GraphicsRenderer::ClearCurrentSurface(Colors::Black);


	Color colors[2] = { (Color(0,0,0,0.5)), Color(0,0,0,0.0f) };

	GraphicsRenderer::ChangeBlendingMode(BlendMode::AllSet);

	for (short y = 0; y < mapSizeTiles; ++y) {
		for (short x = 0; x < mapSizeTiles; ++x) {

			uint8_t state = vision.GetState({ x,y });
			if (state) {
				const Color& c = colors[state - 1];
				GraphicsRenderer::DrawRectangle({ { x * multiplier, y * multiplier }, { multiplier, multiplier} }, c);
			}
		}
	}

	GraphicsRenderer::ChangeBlendingMode(BlendMode::Alpha);
}

void MapSystem::RedrawMinimap(EntityManager& em) {
	const PlayerVision& vision = em.GetPlayerSystem().GetPlayerVision(ActivePlayer);

	if (minimapTerrainTexture.surfaceId == nullptr) {
		GenerateMinimapTerrainTexture();
		minimapTexture = GraphicsRenderer::NewRenderSurface({ minimapTextureSize,minimapTextureSize }, true);
	}

	RedrawMinimapFogOfWar(vision);

	GraphicsRenderer::DrawOnSurface(minimapTexture);
	GraphicsRenderer::Draw(minimapTerrainTexture, { 0,0 });

	if (FogOfWarVisible) {
		GraphicsRenderer::Draw(minimapFowTexture, { 0,0 });
	}

	int end = minimapData.size();
	for (int i = 0; i < end; ++i) {
		const Rectangle16& collider = minimapData.dst[i];
		const Color32& color = minimapData.color[i];

		Rectangle dst = { Vector2Int(collider.position), Vector2Int(collider.size) };
		GraphicsRenderer::DrawRectangle(dst, color);
	}

	GraphicsRenderer::DrawOnCurrentScreen();
}

void MapSystem::DrawMinimap(Rectangle dst)
{
	if (minimapTexture.surfaceId == nullptr) {
		return;
	}

	GraphicsRenderer::Draw(minimapTexture, dst);
}

void MapSystem::DrawGrid(const Camera& camera)
{
	Rectangle camRect = camera.GetRectangle();

	Color color = Colors::Gray;
	color.a = 0.5f;
	Color32 cellColor = Color32(color);
	color = Colors::White;
	color.a = 0.7f;
	Color32 bucketColor = Color32(color);

	Rectangle dst;


	Vector2Int pos = ((camRect.position) / 32) * 32;
	dst.size = { 1, camRect.size.y };

	for (int x = pos.x; x <= pos.x + camRect.size.x + 32; x += 32) {

		dst.position = { x,0 };
		dst.position.x -= camRect.position.x;
		dst.position /= camera.Scale;

		Color32  c = (x % (32 * 32) == 0) ? bucketColor : cellColor;

		GraphicsRenderer::DrawRectangle(dst, c);
	}

	dst.size = { camRect.size.x ,1 };

	for (int y = pos.y; y <= pos.y + camRect.size.y + 32; y += 32) {

		dst.position = { 0,y };
		dst.position.y -= camRect.position.y;
		dst.position /= camera.Scale;

		Color32  c = (y % (32 * 32) == 0) ? bucketColor : cellColor;

		GraphicsRenderer::DrawRectangle(dst, c);
	}

}