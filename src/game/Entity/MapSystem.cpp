#include "MapSystem.h"
#include "EntityManager.h"
#include "../Game.h"
#include "../Platform.h"
#include "../Profiler.h"

static Sprite tile;

void MapSystem::SetSize(Vector2Int16 size)
{
	mapSize = size;
	gridSize = size / 32;
	minimapTextureSize = size.x / 32;

	tile = Game::AssetLoader.LoadAtlas("tileset_tile.t3x")->GetSprite(0);
}

void MapSystem::UpdateMap(EntityManager& em)
{
	SectionProfiler p("UpdateMap");

	minimapData.clear();

	const std::vector<PlayerInfo>& players = em.GetPlayerSystem().GetPlayers();

	Color32 colors[16];
	for (int i = 0; i < players.size(); ++i) {
		colors[i] = players[i].color;
	}
	colors[ActivePlayer] = Color32(Colors::MapFriendly);
	const auto vision = em.GetPlayerSystem().GetPlayerVision(ActivePlayer);


	for (EntityId id : em.UnitArchetype.Archetype.GetEntities()) {
		if (em.FlagComponents.GetComponent(id).test(ComponentFlags::RenderEnabled) &&
			(!em.HiddenArchetype.Archetype.HasEntity(id)))
		{
			const auto& def = em.UnitArchetype.UnitComponents.GetComponent(id).def;

			Rectangle16 dst;
			dst.position += em.PositionComponents.GetComponent(id);
			dst.position = (dst.position >> 5) + def->Graphics->MinimapBB.position;
			dst.size = def->Graphics->MinimapBB.size;
			dst.Restrict(Vector2Int16(0, 0), gridSize - Vector2Int16(1, 1));

			PlayerId owner = em.UnitArchetype.OwnerComponents.GetComponent(id);

			bool visible = vision.IsVisible(dst);

			if (!visible && FogOfWarVisible)
				continue;

			minimapData.dst.push_back(dst);
			minimapData.color.push_back(colors[owner]);
		}
	}
}

void MapSystem::UpdateVisibleRenderEntities(EntityManager& em) {
	auto& vision = em.GetPlayerSystem().GetPlayerVision(ActivePlayer);

	removedEntities.clear();

	for (EntityId id : em.RenderArchetype.Archetype.GetEntities()) {
		auto dst = em.RenderArchetype.BoundingBoxComponents.GetComponent(id);
		dst.position = dst.position >> 5;
		dst.size = dst.size >> 5;

		dst.Restrict(Vector2Int16(0, 0), gridSize - Vector2Int16(1, 1));

		bool visible =  vision.IsVisible(dst);

		if (!visible && FogOfWarVisible) {
			removedEntities.push_back(id);
		}
	}


	addedEntities.clear();

	for (EntityId id : em.HiddenArchetype.Archetype.GetEntities()) {
		auto dst = em.RenderArchetype.BoundingBoxComponents.GetComponent(id);
		Vector2Int16 pos = em.PositionComponents.GetComponent(id);
		dst.SetCenter(pos);
		dst.position = dst.position >> 5;
		dst.size = dst.size >> 5;

		dst.Restrict(Vector2Int16(0, 0), gridSize - Vector2Int16(1, 1));

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
	auto& vision = em.GetPlayerSystem().GetPlayerVision(ActivePlayer);

	removedEntities.clear();

	for (EntityId id : em.UnitArchetype.RenderArchetype.Archetype.GetEntities()) {
		auto dst = em.UnitArchetype.RenderArchetype.BoundingBoxComponents.GetComponent(id);
		dst.position = dst.position >> 5;
		dst.size = dst.size >> 5;

		dst.Restrict(Vector2Int16(0, 0), gridSize - Vector2Int16(1, 1));

		bool visible = vision.IsVisible(dst);

		if (!visible && FogOfWarVisible) {
			removedEntities.push_back(id);
		}
	}


	addedEntities.clear();

	for (EntityId id : em.UnitArchetype.HiddenArchetype.Archetype.GetEntities()) {
		auto dst = em.UnitArchetype.RenderArchetype.BoundingBoxComponents.GetComponent(id);
		Vector2Int16 pos = em.PositionComponents.GetComponent(id);
		dst.SetCenter(pos);
		dst.position = dst.position >> 5;
		dst.size = dst.size >> 5;

		dst.Restrict(Vector2Int16(0, 0), gridSize - Vector2Int16(1, 1));

		bool visible = vision.IsVisible(dst);

		if (visible || !FogOfWarVisible) {
			addedEntities.push_back(id);
			em.FlagComponents.GetComponent(id).set(ComponentFlags::RenderChanged);
		}
	}

	em.UnitArchetype.RenderArchetype.Archetype.RemoveSortedEntities(removedEntities);
	em.UnitArchetype.HiddenArchetype.Archetype.AddSortedEntities(removedEntities);
	em.UnitArchetype.HiddenArchetype.Archetype.RemoveSortedEntities(addedEntities);
	em.UnitArchetype.RenderArchetype.Archetype.AddSortedEntities(addedEntities);
}

void MapSystem::UpdateVisibleEntities(EntityManager& em) {
	SectionProfiler p("UpdateVisibleEntities");

	UpdateVisibleRenderEntities(em);
	UpdateVisibleRenderUnits(em);
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

			Platform::Draw(tile, dst);
		}
	}
}

void MapSystem::DrawFogOfWar(const Camera& camera) {
	if (!FogOfWarVisible || minimapFowTexture.textureId == nullptr)
		return;

	static constexpr const int Upscale = 4;

	Vector2Int16 FogOfWarTextureSize = Vector2Int16(minimapTextureSize, minimapTextureSize);
	FogOfWarTextureSize *= Upscale;

	static Sprite fowDownscaleSprite;
	Platform::ChangeBlendingMode(BlendMode::FullOverride);

	if (fogOfWarTexture.textureId == nullptr) {
		fogOfWarTexture = Platform::NewTexture(Vector2Int(FogOfWarTextureSize));
		Image t = Platform::NewTexture(Vector2Int(minimapTextureSize / 2));
		fowDownscaleSprite = Platform::NewSprite(t, { {0,0},Vector2Int16(minimapTextureSize / 2) });
	}

	Rectangle16 camRect = camera.GetRectangle16();

	Platform::DrawOnTexture(fowDownscaleSprite.image.textureId);
	Platform::ClearBuffer(Colors::Transparent);

	// downscale minimap fow
	Platform::Draw({ { {0,0}, {minimapTextureSize, minimapTextureSize}}, minimapFowTexture },
		{ {0,0},{fowDownscaleSprite.rect.size.x,fowDownscaleSprite.rect.size.y} });

	Platform::DrawOnTexture(fogOfWarTexture.textureId);
	Platform::ClearBuffer(Colors::Transparent);

	// upscale downscaled minimap fow
	Platform::Draw(fowDownscaleSprite,
		{ {0,0},{FogOfWarTextureSize.x,FogOfWarTextureSize.y} });

	Platform::DrawOnTexture(nullptr);
	Platform::ChangeBlendingMode(BlendMode::Alpha);

	static constexpr const int CamDownscale = 32 / Upscale;
	Rectangle16 src = { (camRect.position / CamDownscale), (camRect.size / CamDownscale) };
	Sprite fowSprite = Platform::NewSprite(fogOfWarTexture, src);


	Platform::Draw(fowSprite, { {0,0},{400,240} });
}

void MapSystem::GenerateMiniampTerrainTexture() {
	Rectangle mapBounds = { {0,0}, Vector2Int(mapSize) };

	minimapTerrainTexture = Platform::NewTexture({ minimapTextureSize,minimapTextureSize });
	Platform::DrawOnTexture(minimapTerrainTexture.textureId);

	Vector2 upscale = Vector2(mapBounds.size) / Vector2(minimapTextureSize, minimapTextureSize);

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

void MapSystem::RenderMinimapFogOfWar(const PlayerVision& vision) {

	if (minimapFowTexture.textureId == nullptr) {
		minimapFowTexture = Platform::NewTexture({ minimapTextureSize,minimapTextureSize });
	}

	int mapSizeTiles = (int)mapSize.x / 32;
	int multiplier = minimapTextureSize / mapSizeTiles;

	Platform::DrawOnTexture(minimapFowTexture.textureId);
	Platform::ClearBuffer(Colors::Black);


	Color32 colors[2] = { Color32(Color(0,0,0,0.6)), Color32(0) };

	Platform::ChangeBlendingMode(BlendMode::FullOverride);

	for (short y = 0; y < mapSizeTiles; ++y) {
		for (short x = 0; x < mapSizeTiles; ++x) {

			uint8_t state = vision.GetState({ x,y });
			if (state) {
				const Color32& c = colors[state - 1];
				Platform::DrawRectangle({ { x * multiplier, y * multiplier }, { multiplier, multiplier} }, c);
			}
		}
	}

	Platform::ChangeBlendingMode(BlendMode::Alpha);
}

void MapSystem::RedrawMinimap(EntityManager& em) {
	const PlayerVision& vision = em.GetPlayerSystem().GetPlayerVision(ActivePlayer);

	if (minimapTerrainTexture.textureId == nullptr) {
		GenerateMiniampTerrainTexture();
	}

	if (minimapTexture.textureId == nullptr) {
		minimapTexture = Platform::NewTexture({ minimapTextureSize,minimapTextureSize }, true);
	}

	RenderMinimapFogOfWar(vision);

	Platform::DrawOnTexture(minimapTexture.textureId);

	Sprite fullMapSprite;
	fullMapSprite.rect = { {0,0}, Vector2Int16(minimapTextureSize) };
	fullMapSprite.image = minimapTerrainTexture;
	Platform::Draw(fullMapSprite, { {0,0}, Vector2Int(minimapTextureSize) });

	if (FogOfWarVisible) {
		fullMapSprite.image = minimapFowTexture;
		Platform::Draw(fullMapSprite, { {0,0}, Vector2Int(minimapTextureSize) });
	}

	int end = minimapData.size();
	for (int i = 0; i < end; ++i) {
		const Rectangle16& collider = minimapData.dst[i];
		const Color32& color = minimapData.color[i];

		Rectangle dst = { Vector2Int(collider.position), Vector2Int(collider.size) };
		Platform::DrawRectangle(dst, color);
	}

	Platform::DrawOnTexture(nullptr);
}

void MapSystem::DrawMinimap(Rectangle dst)
{
	if (minimapTexture.textureId == nullptr) {
		return;
	}

	Sprite minimapSprite;
	minimapSprite.rect = { {0,0}, Vector2Int16(minimapTextureSize,minimapTextureSize) };
	minimapSprite.image = minimapTexture;

	Platform::Draw(minimapSprite, dst);
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

		Platform::DrawRectangle(dst, c);
	}

	dst.size = { camRect.size.x ,1 };

	for (int y = pos.y; y <= pos.y + camRect.size.y + 32; y += 32) {

		dst.position = { 0,y };
		dst.position.y -= camRect.position.y;
		dst.position /= camera.Scale;

		Color32  c = (y % (32 * 32) == 0) ? bucketColor : cellColor;

		Platform::DrawRectangle(dst, c);
	}

}