#pragma once

#include "Scene.h"
#include "Entity/EntityManager.h"

class PerformanceTestScene : public Scene {
public:
	virtual void Start() override;
	virtual void Update() override;
	virtual void Draw() override;
private:
	Camera camera;
	EntityManager em;
};