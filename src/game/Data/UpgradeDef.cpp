#include "UpgradeDef.h"
#include "../Assets.h"
#include "GameDatabase.h"

const ImageFrame& UpgradeDef::GetIcon() const
{
	return GameDatabase::instance->GetIcon(IconId);
}
