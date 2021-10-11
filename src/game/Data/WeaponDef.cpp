#include "WeaponDef.h"
#include "../Assets.h"
#include "GameDatabase.h"

const ImageFrame& WeaponDef::GetIcon() const
{
	return GameDatabase::instance->GetIcon(IconId);
}

const SoundSetDef* WeaponDef::GetSpawnSound() const
{
	if (SpawnSound < 0)
		return nullptr;

	return &GameDatabase::instance->SoundSetDefs[SpawnSound];
}
