#pragma once


#include "Scene.h"

class BootScene : public Scene
{
public:
	BootScene();
	virtual void Start() override;
	virtual void Draw() override;
	virtual ~BootScene()  override;
};