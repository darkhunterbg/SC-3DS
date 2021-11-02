#pragma once

#include "Scene.h"

class MainMenuSceneView;

class MainMenuScene : public Scene {
private:
	MainMenuSceneView* _view = nullptr;
public:
	MainMenuScene();
	~MainMenuScene();
	virtual void Start() override;
	virtual void Stop() override;
	virtual void Frame(TimeSlice& frameBudget) override;
};