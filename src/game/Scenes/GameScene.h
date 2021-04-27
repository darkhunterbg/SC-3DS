#pragma once

#include "Scene.h"
#include "../Camera.h"

class MapSystem;


class GameScene : public Scene {

public:
	virtual ~GameScene() override;

	virtual void Start() override;
	virtual void Update() override;
	virtual void Draw() override;

private:
	MapSystem* mapSystem;
	Camera camera;
};