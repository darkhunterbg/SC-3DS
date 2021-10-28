#include "UnitPortraitPanel.h"
#include "../GUI/GUI.h"
#include "../Entity/EntityUtil.h"
#include "../Entity/EntityManager.h"

#include "../Data/GameDatabase.h"
#include "../Engine/GraphicsRenderer.h"
#include "../Platform.h"
#include "../Engine/JobSystem.h"

static constexpr const int NoiseTime = 10;

static constexpr const int TextureSize = 64;

static UnitPortraitPanel* _upp;

static int RandomNoise(Vector2 pos, float input)
{
	// https://www.ronja-tutorials.com/post/024-white-noise/

	pos.x = std::sin(pos.x);
	pos.y = std::sin(pos.y);
	float random = Vector2::Dot(pos, Vector2(12.9898f, 78.233f));
	random *= input;
	float f;
	random = std::fabs(std::modf(std::sin(random) * 143758.5453f, &f));
	random = std::sin(random) * 143758.5453;

	return (int)(random * 255);
}


UnitPortraitPanel::UnitPortraitPanel()
{
	_upp = this;

	_noise = GraphicsRenderer::NewTexture({ TextureSize,TextureSize }, true);
	_textureData = (uint8_t*)Platform::PlatformAlloc(TextureSize * TextureSize * 4);
}

UnitPortraitPanel::~UnitPortraitPanel()
{
	Platform::PlatformFree(_textureData);
	Platform::DestroyTexture(_noise->GetTextureId());
}

void UnitPortraitPanel::RegenerateNoiseTextureJob(int start, int end)
{
	for (int o = start; o < end; ++o)
	{
		int x = o % TextureSize;
		int y = o / TextureSize;
		// Add 1 because in 0 coordinates noise is constant value
		int gen = RandomNoise(Vector2(x + 1, y + 1), _upp->_noiseInput);
		int i = o * 4;

#ifdef _3DS
		_upp->_textureData[i++] = 255;
		_upp->_textureData[i++] = gen;
		_upp->_textureData[i++] = gen;
		_upp->_textureData[i++] = gen;
#else
		_upp->_textureData[i++] = gen;
		_upp->_textureData[i++] = gen;
		_upp->_textureData[i++] = gen;
		_upp->_textureData[i++] = 255;
#endif
	}
}

void UnitPortraitPanel::RegenerateNoiseTexture(float input)
{
	_noiseInput = input;

	JobSystem::RunJob(TextureSize * TextureSize, 512, RegenerateNoiseTextureJob);

	Platform::UpdateTexture(_noise->GetTextureId(), { {0,0},{TextureSize,TextureSize} }, { _textureData, TextureSize * TextureSize * 4 });
}

void UnitPortraitPanel::Draw(EntityId id)
{
	if (id == Entity::None) return;

	EntityManager& em = EntityUtil::GetManager();

	if (!em.UnitSystem.IsUnit(id)) return;

	if (id != _unit)
	{
		_portraitId = 0;
		_unit = id;
		_noiseCooldown = NoiseTime;
	}

	const UnitComponent& unit = em.UnitSystem.GetComponent(id);

	const UnitPortraitDef* portrait = unit.def->Art.GetPortrait();
	if (portrait && portrait->GetIdleClips().Size() > _portraitId)
	{
		bool ended = GUIVideo::DrawVideo("portrait", portrait->GetIdleClip(_portraitId), true);
		if (ended)
		{
			_portraitId = (_portraitId + 1) % unit.def->Art.GetPortrait()->IdleClipCount;
		}
	}

	if (_noiseCooldown > 0 && NoiseTime > 0 && _noise != nullptr)
	{
		float alpha = (float)_noiseCooldown / (float)NoiseTime;

		// Alternativse:
		// Coroutine to distribute generation over 2 frames
		// One texutre, with flipping
		if (_noiseCooldown % 2 == 0)
			RegenerateNoiseTexture(alpha);

		GUIImage::DrawTexture(*_noise);

		--_noiseCooldown;
	}
}
