#include "AbilityDef.h"
#include "GameDatabase.h"

const ImageFrame& AbilityDef::AbilityArt::GetIcon() const
{
	return GameDatabase::instance->GetIcon(IconId);
}
