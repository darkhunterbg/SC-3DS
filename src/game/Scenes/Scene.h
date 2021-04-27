#pragma once

class Scene {
public :
	virtual ~Scene() = 0;

	virtual void Start() {}
	virtual void Stop() {}

	virtual void Update() {}
	virtual void Draw() {}
};