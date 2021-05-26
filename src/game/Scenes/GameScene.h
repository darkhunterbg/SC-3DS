#pragma once

#include "Scene.h"
#include "../Camera.h"

#include "../Entity/Entity.h"

class Cursor;
class GameView;
class EntityManager;

class GameScene : public Scene {

public:
	GameScene();
	virtual ~GameScene() override;

	virtual void Start() override;
	virtual void Update() override;
	virtual void Draw() override;

private:
	GameView* view;
	Camera camera;
	EntityManager* entityManager;
};