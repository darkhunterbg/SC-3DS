#pragma once


#include "Scene.h"
#include "../Coroutine.h"

class BootSceneView;

class BootScene : public Scene
{
private: 
	int _frameCounter = 0;
	bool _ready = false;
	Coroutine _loadCrt;
	BootSceneView* _view;
public:
	BootScene();
	virtual void Start() override;
	virtual void Stop() override;
	virtual void Update() override;

	virtual void Draw() override;
	virtual ~BootScene()  override;

	void StartGame();
	bool IsLoadCompleted() const { return _ready; }
};