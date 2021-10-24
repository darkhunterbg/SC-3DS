#pragma once

#include <string>

class Scene {
protected:
	 std::string _id;
public :
	virtual ~Scene() = 0;

	virtual void Start() {}
	virtual void Stop() {}

	virtual void Update() {}
	virtual void Draw() {}

	const  std::string& GetId() const
	{
		return _id;
	}

};