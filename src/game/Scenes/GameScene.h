#pragma once

#include "Scene.h"
#include "../Coroutine.h"

class GameSceneView;
class EntityManager;

class GameScene : public Scene {

public:
	GameScene();
	virtual ~GameScene() override;

	virtual void Start() override;
	virtual void Stop() override;
	virtual void Frame() override;
	virtual void OnPlatformChanged() override;

	EntityManager& GetEntityManager() { return *_entityManager; }
	GameSceneView& GetView() { return *_view; }

	double GameSpeed = 1.0;
private:

	GameSceneView* _view;
	EntityManager* _entityManager;
	Coroutine _updateCrt;
};