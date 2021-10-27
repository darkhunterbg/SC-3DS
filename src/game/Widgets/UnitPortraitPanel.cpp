#include "UnitPortraitPanel.h"
#include "../GUI/GUI.h""
#include "../Entity/EntityUtil.h"
#include "../Entity/EntityManager.h"

#include "../Data/GameDatabase.h"
#include "../Engine/GraphicsRenderer.h"

static constexpr const int NoiseTime = 6;

UnitPortraitPanel::UnitPortraitPanel()
{

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

	//if (_noiseCooldown > 0 && NoiseTime > 0 && _noiseImage != nullptr)
	//{
	//	float alpha = (float)_noiseCooldown / (float)NoiseTime;

	//	Color c = Color(1, 1, 1, alpha);

	//	GUIImage::DrawImage(*_noiseImage, 0, c);

	//	--_noiseCooldown;
	//}
}
