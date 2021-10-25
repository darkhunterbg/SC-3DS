#pragma once

#include "../Assets.h"
#include "../Widgets/Cursor.h"

class MainMenuScene;

class MainMenuSceneView {

private:
	MainMenuScene* _scene;
	VideoClip* _single, * _singleon, * _multi, * _multion, * _exit, * _exiton;
	Font* _mmFont;
	Cursor _cursor;
public:
	MainMenuSceneView(MainMenuScene* scene);
	~MainMenuSceneView();
	void Draw();
};