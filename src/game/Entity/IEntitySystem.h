#pragma once

#include "Entity.h"

class IEntitySystem {

public:
	
	virtual void DeleteEntities(std::vector<EntityId>& entities) = 0;
	virtual size_t ReportMemoryUsage() = 0;
};