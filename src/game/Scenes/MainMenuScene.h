#pragma once

#include "Scene.h"

class MainMenuSceneView;

class MainMenuScene : public Scene {
private:
	MainMenuSceneView* _view;
public:
	virtual void Start() override;
	virtual void Stop() override;
	virtual void Draw() override;
};