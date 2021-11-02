#pragma once


#include "Scene.h"
#include "../Coroutine.h"

class BootSceneView;

class BootScene : public Scene
{
private: 
	bool _ready = false;
	Coroutine _loadCrt;
	BootSceneView* _view = nullptr;
	Scene* _nextScene = nullptr;
	int _nextSceneDelay = 30;

public:
	BootScene(Scene* next = nullptr);
	virtual void Start() override;
	virtual void Stop() override;

	virtual void Frame(TimeSlice& frameBudget) override;
	virtual ~BootScene()  override;

	void StartGame();
	bool IsLoadCompleted() const { return _ready; }
};