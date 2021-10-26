#pragma once

#include "../Assets.h"

class BootScene;

class BootSceneView {

private:
	BootScene* _scene = nullptr;
	int _frameCounter = 0;
	Texture* _title;
public: 
	BootSceneView(BootScene* scene);
	~BootSceneView();

	void Draw();
};