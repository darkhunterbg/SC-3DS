#pragma once

#include "../Assets.h"

class MainMenuScene;

class MainMenuSceneView {

private:
	MainMenuScene* _scene;
	VideoClip* _single, * _singleon, * _multi, * _multion, * _exit, * _exiton;
	Font* _mmFont;
public:
	MainMenuSceneView(MainMenuScene* scene);
	~MainMenuSceneView();
	void Draw();
};