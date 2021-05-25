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
	static SpriteFrameAtlas* unit_thingy_tbangl;
	static SpriteFrameAtlas* unit_thingy_tbangx;
	static SpriteFrameAtlas* unit_neutral_min01;
	static SpriteFrameAtlas* unit_neutral_min01sha;
	static SpriteFrameAtlas* unit_bullet_tspark;
	static SpriteFrameAtlas* cursor;

	static const SpriteFrameAtlas* Load_unit_terran_marine();
	static const SpriteFrameAtlas* Load_unit_terran_tmashad();
	static const SpriteFrameAtlas* Load_unit_terran_control();
	static const SpriteFrameAtlas* Load_unit_terran_tccshad();
	static const SpriteFrameAtlas* Load_unit_terran_scv();
	static const SpriteFrameAtlas* Load_unit_thingy_tbangs();
	static const SpriteFrameAtlas* Load_unit_thingy_tbangl();
	static const SpriteFrameAtlas* Load_unit_thingy_tbangx();
	static const SpriteFrameAtlas* Load_unit_neutral_min01();
	static const SpriteFrameAtlas* Load_unit_neutral_min01sha();
	static const SpriteFrameAtlas* Load_unit_bullet_tspark();
	static const SpriteFrameAtlas* Load_cursor();
};
