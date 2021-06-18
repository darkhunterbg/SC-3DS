#include "AbilityDef.h"
#include "GameDatabase.h"

const ImageFrame& AbilityDef::Art::GetIcon() const
{
	return GameDatabase::instance->GetCommandIcons(IconId);
}
