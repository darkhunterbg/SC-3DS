#pragma once

#include <string>
#include "../TimeSlice.h"

class Scene {
protected:
	 std::string _id;
public :
	virtual ~Scene() = 0;

	virtual void Start() {}
	virtual void Stop() {}

	virtual void Frame(TimeSlice& frameBudget) {}

	virtual void OnPlatformChanged() {}

	const  std::string& GetId() const
	{
		return _id;
	}

};