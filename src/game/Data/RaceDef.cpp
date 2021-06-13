#include "RaceDef.h"

#include "../Engine/AssetLoader.h"


void RaceDef::LoadResourses() {
	AdvisorErrorSounds.LoadSoundClips();
	AdvisorUpdateSounds.LoadSoundClips();

	CommandIcons = &AssetLoader::GetDatabase().GetImage(commandIconsPath);
	ConsoleLowerSprite = AssetLoader::GetDatabase().GetImage(consolePath).GetFrame(0);
	ConsoleUpperSprite = AssetLoader::GetDatabase().GetImage(consolePath + "up").GetFrame(0);


	SupplyIcon  = AssetLoader::GetDatabase().GetImage(supplyIconPath).GetFrame(0);
	GasIcon = AssetLoader::GetDatabase().GetImage(gasIconPath).GetFrame(0);
		//const SpriteAtlas* icons = AssetLoader::LoadAtlas("game_icons.t3x");
		//SupplyIcon = icons->GetSprite(SupplyIconId);

		//GasIcon = icons->GetSprite(GasIconId);

		for (auto& musicDef : GameMusic)
			musicDef.Load();
}