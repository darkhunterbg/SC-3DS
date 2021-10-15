#pragma once


#include "Scene.h"
class Coroutine;

class BootScene : public Scene
{
private: 
	int _frameCounter = 0;

	Coroutine* _loadingCrt = nullptr;
public:
	BootScene();
	virtual void Start() override;
	virtual void Stop() override;
	virtual void Update() override;

	virtual void Draw() override;
	virtual ~BootScene()  override;
};