#pragma once

class IMainMenuSceneView {

protected:
	class MainMenuScene* _scene;
public:
	IMainMenuSceneView(class MainMenuScene* scene) :_scene(scene) {}
	virtual ~IMainMenuSceneView() = 0;
	virtual void Draw() = 0;

	virtual void OnShow() {}
	// returns true when done, then view will be swithed
	virtual bool DoneHiding() { return true; }
};