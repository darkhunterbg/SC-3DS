#pragma once

#include "../Entity/Entity.h"

#include "../Assets.h"
#include "../Random.h"

class Texture;

class UnitPortraitPanel {
private:
	int _portraitId = 0;
	EntityId _unit = Entity::None;
	VideoClip* _playClip = nullptr;
	int _noiseCooldown;
	const Texture* _noise = nullptr;

	uint8_t* _textureData = nullptr;

	float _noiseInput;

	Random _rnd;

	static void RegenerateNoiseTextureJob(int start, int end);
	void RegenerateNoiseTexture(float input);
public:
	void ChatUnit(EntityId unit, bool newUnit);
	
	UnitPortraitPanel();
	~UnitPortraitPanel();

	void Draw(EntityId unit);
};