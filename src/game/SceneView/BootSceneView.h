#pragma once

#include "../Assets.h"

class BootScene;

class BootSceneView {

private:
	BootScene* _scene = nullptr;
	int _frameCounter = 0;
	int _nextSceneDelay = 30;
	Texture* _title;
public: 
	BootSceneView(BootScene* scene);
	~BootSceneView();

	void Draw();
};