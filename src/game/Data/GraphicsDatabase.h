#pragma once

#include "GraphicsDef.h"
#include <vector>


class GraphicsDatabase {
private:
	GraphicsDatabase() = delete;
	~GraphicsDatabase() = delete;
public:
	static UnitGraphicsDef Marine, SCV, CommandCenter;

	static std::vector<UnitGraphicsDef*> Units;

	static void Init();
	static void LoadAllGraphicsResources();
};