#pragma once

#include "../Assets.h"
#include "../Widgets/Cursor.h"
#include "IMainMenuSceneView.h"

class MainMenuView : public IMainMenuSceneView {

private:
	VideoClip* _single, * _singleon, * _multi, * _multion, *_editor, *_editoron, * _exit, * _exiton;

	void DrawGenric();
	void DrawN3DS();

	int _menuIndex = 0;
public:
	MainMenuView(class MainMenuScene* scene);
	virtual ~MainMenuView() override;
	virtual void Draw() override;
};