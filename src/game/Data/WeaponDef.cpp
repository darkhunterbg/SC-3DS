#include "WeaponDef.h"
#include "../Assets.h"
#include "GameDatabase.h"

const ImageFrame& WeaponDef::GetIcon() const
{
	return GameDatabase::instance->GetIcon(IconId);
}
