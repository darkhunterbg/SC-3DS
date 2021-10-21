#pragma once

class BootScene;

class BootSceneView {

private:
	BootScene* _scene = nullptr;
	int _frameCounter = 0;
public: 
	BootSceneView(BootScene* scene);

	void Draw();
};