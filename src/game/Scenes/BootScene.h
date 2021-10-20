#pragma once


#include "Scene.h"
#include "../Coroutine.h"

class BootScene : public Scene
{
private: 
	int _frameCounter = 0;
	bool _ready = false;
	Coroutine _loadCrt;
public:
	BootScene();
	virtual void Start() override;
	virtual void Stop() override;
	virtual void Update() override;

	virtual void Draw() override;
	virtual ~BootScene()  override;
};