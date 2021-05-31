#include "RaceDef.h"

#include "../Engine/AssetLoader.h"

void RaceDef::LoadResourses() {
	ConsoleSprite.Load();
	AdvisorErrorSounds.LoadSoundClips();
	AdvisorUpdateSounds.LoadSoundClips();

	CommandIconsLoad();

	const SpriteAtlas* icons = AssetLoader::LoadAtlas("game_icons.t3x");
	SupplyIcon = icons->GetSprite(SupplyIconId);

	GasIcon = icons->GetSprite(GasIconId);

	for (auto& musicDef : GameMusic)
		musicDef.Load();
}