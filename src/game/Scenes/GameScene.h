#pragma once

#include "Scene.h"


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
private:
	GameSceneView* _view;
	EntityManager* _entityManager;
};