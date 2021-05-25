#pragma once

#include "GraphicsDef.h"
#include <vector>


struct CursorGraphics {
	AnimationClip arrow, drag, illegal, magg, magr, magy, scrolld, scrolldl, scrolldr, scrolll, scrollr, scrollu, scrollul, scrollur, targg, targn, targr, targy;
	const AnimationClip* scrollAnim[9];

	void Load();
};

class GraphicsDatabase {
private:
	GraphicsDatabase() = delete;
	~GraphicsDatabase() = delete;
public:
	static CursorGraphics Cursor;

	static UnitGraphicsDef Marine, SCV, CommandCenter, Minerals1;

	static std::vector<UnitGraphicsDef*> Units;

	static void Init();
	static void LoadAllGraphicsResources();
};