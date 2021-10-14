#pragma once

#include "Scene.h"
#include "../GUI/GameView.h"

#include "../Entity/Entity.h"

class EntityManager;

class GameScene : public Scene {

public:
	GameScene();
	virtual ~GameScene() override;

	virtual void Start() override;
	virtual void Update() override;
	virtual void Draw() override;

private:
	GameView view;
	EntityManager* entityManager;
};