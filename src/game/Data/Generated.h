#pragma once

#include "../Assets.h"
#include <vector>

class SpriteDatabase{

	private:
	SpriteDatabase() = delete;
	~SpriteDatabase() = delete;

public:


	static SpriteFrameAtlas* unit_terran_marine;
	static SpriteFrameAtlas* unit_terran_tmashad;
	static SpriteFrameAtlas* unit_terran_control;
	static SpriteFrameAtlas* unit_terran_tccshad;
	static SpriteFrameAtlas* unit_terran_scv;
	static SpriteFrameAtlas* unit_thingy_tbangs;

	static const SpriteFrameAtlas* Load_unit_terran_marine();
	static const SpriteFrameAtlas* Load_unit_terran_tmashad();
	static const SpriteFrameAtlas* Load_unit_terran_control();
	static const SpriteFrameAtlas* Load_unit_terran_tccshad();
	static const SpriteFrameAtlas* Load_unit_terran_scv();
	static const SpriteFrameAtlas* Load_unit_thingy_tbangs();
};
