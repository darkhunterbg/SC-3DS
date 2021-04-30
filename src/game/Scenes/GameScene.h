#pragma once

#include "Scene.h"
#include "../Camera.h"

class MapSystem;
class GameHUD;
class Cursor;
class EntityManager;

class GameScene : public Scene {

public:
	GameScene();
	virtual ~GameScene() override;

	virtual void Start() override;
	virtual void Update() override;
	virtual void Draw() override;

private:
	MapSystem* mapSystem;
	Camera camera;
	GameHUD* hud;
	Cursor* cursor;
	EntityManager* entityManager;
	unsigned logicalFrame = 0;
	unsigned frameCounter = 0;

	void LogicalUpdate();
};