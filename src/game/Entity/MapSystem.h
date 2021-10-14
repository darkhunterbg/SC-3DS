#pragma once

#include "../MathLib.h"
#include "Entity.h"
#include "IEntitySystem.h"

#include "../Engine/GraphicsPrimitives.h"
#include "PlayerSystem.h"

class EntityManager;
class Camera;
class PlayerVision;

class MapSystem : public IEntitySystem {
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

	void RedrawMinimapFogOfWar(const PlayerVision& vision);
public:
	PlayerId ActivePlayer = { 1 };
	bool FogOfWarVisible = true;

	void SetSize(Vector2Int16 size);

	void DrawMap(const Camera& camera);
	void DrawFogOfWar(EntityManager& em, const Camera& camera);
};