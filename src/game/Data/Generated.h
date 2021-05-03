#pragma once

#include "../Assets.h"
#include <vector>

class SpriteDatabase{

	private:
	SpriteDatabase() = delete;
	~SpriteDatabase() = delete;

public:

	static std::vector<const SpriteFrameAtlas*> Units;


	static SpriteFrameAtlas* unit_terran_marine;
	static const SpriteFrameAtlas* Load_unit_terran_marine();

};
