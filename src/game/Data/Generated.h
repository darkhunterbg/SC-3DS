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
	static SpriteFrameAtlas* unit_thingy_o022;
	static SpriteFrameAtlas* unit_wirefram_wirefram_000;
	static SpriteFrameAtlas* unit_wirefram_wirefram_106;
	static SpriteFrameAtlas* unit_thingy_o146;
	static SpriteFrameAtlas* unit_cmdbtns_tcmdbtns;
	static SpriteFrameAtlas* unit_cmdbtns_pcmdbtns;
	static SpriteFrameAtlas* unit_cmdbtns_zcmdbtns;
	static SpriteFrameAtlas* unit_cmdbtns_cmdicons;
	static SpriteFrameAtlas* unit_thingy_ofirec;
	static SpriteFrameAtlas* unit_thingy_ofirev;
	static SpriteFrameAtlas* unit_thingy_ofiref;

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
	static const SpriteFrameAtlas* Load_unit_thingy_o022();
	static const SpriteFrameAtlas* Load_unit_wirefram_wirefram_000();
	static const SpriteFrameAtlas* Load_unit_wirefram_wirefram_106();
	static const SpriteFrameAtlas* Load_unit_thingy_o146();
	static const SpriteFrameAtlas* Load_unit_cmdbtns_tcmdbtns();
	static const SpriteFrameAtlas* Load_unit_cmdbtns_pcmdbtns();
	static const SpriteFrameAtlas* Load_unit_cmdbtns_zcmdbtns();
	static const SpriteFrameAtlas* Load_unit_cmdbtns_cmdicons();
	static const SpriteFrameAtlas* Load_unit_thingy_ofirec();
	static const SpriteFrameAtlas* Load_unit_thingy_ofirev();
	static const SpriteFrameAtlas* Load_unit_thingy_ofiref();
};
