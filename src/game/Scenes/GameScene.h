#pragma once

#include "Scene.h"
#include "../Camera.h"

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
	Camera camera;
	GameHUD* hud;
	Cursor* cursor;
	EntityManager* entityManager;
	unsigned logicalFrame = 0;
	unsigned frameCounter = 2;
	unsigned frameCounter2 = 0;

	void LogicalUpdate();
	void AuxilaryUpdate();
};