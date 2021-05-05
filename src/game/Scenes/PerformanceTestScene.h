#pragma once

#include "Scene.h"

class PerformanceTestScene : public Scene {
public:
	virtual void Start() override;
	virtual void Update() override;
	virtual void Draw() override;

};