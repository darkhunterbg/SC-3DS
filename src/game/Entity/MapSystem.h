#pragma once

#include "../MathLib.h"
#include "Entity.h"
#include "IEntitySystem.h"

#include "../Engine/GraphicsPrimitives.h"
#include "EntityComponentMap.h"
#include "PlayerSystem.h"

#include <vector>

class EntityManager;
class Camera;
class PlayerVision;


class MapSystem : public IEntitySystem {
	struct VisionOccludedComponent {

	};

private:

	Vector2Int16 _mapSize;
	Vector2Int16 _gridSize;
	short _minimapTextureSize = 0;

	// Inherited via IEntitySystem
	virtual void DeleteEntities(std::vector<EntityId>& entities) override;
	virtual size_t ReportMemoryUsage() override;

	RenderSurface _minimapTexture = { 0 };
	RenderSurface _minimapFowTexture = { 0 };
	RenderSurface _minimapTerrainTexture = { 0 };

	RenderSurface _fogOfWarTexture = { 0 };
	RenderSurface _fowDownscaleTexture = { 0 };

	std::vector<Rectangle16> _halfLitTiles;
	std::vector<Rectangle16> _fullLitTiles;


	EntityComponentMap<VisionOccludedComponent> _occludedComponents;

	void RedrawMinimapFogOfWar(const PlayerVision& vision);
	void GenerateMinimapTerrainTexture();
public:
	PlayerId ActivePlayer = { 1 };
	bool FogOfWarVisible = true;

	void SetVisionOccluded(EntityId id, bool occluded);

	inline Vector2Int16 GetSize() const
	{
		return _mapSize;
	}

	void SetSize(Vector2Int16 size);

	void DrawOffscreenData(EntityManager& em);

	void UpdateVisibleObjects(EntityManager& em);

	void DrawMap(const Camera& camera);
	void DrawFogOfWar(EntityManager& em, const Camera& camera);
	void DrawMinimap(Rectangle dst);
};