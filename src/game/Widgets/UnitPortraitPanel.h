#pragma once

#include "../Entity/Entity.h"

#include "../Assets.h"

class Texture;

class UnitPortraitPanel {
private:
	int _portraitId = 0;
	EntityId _unit = Entity::None;
	int _noiseCooldown;
	const Texture* _noise = nullptr;

	uint8_t* _textureData = nullptr;

	float _noiseInput;
	static void RegenerateNoiseTextureJob(int start, int end);
	void RegenerateNoiseTexture(float input);
public:
	UnitPortraitPanel();
	~UnitPortraitPanel();
	void Draw(EntityId unit);
};