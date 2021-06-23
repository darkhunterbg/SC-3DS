#include "RaceDef.h"

#include "GameDatabase.h"
#include "../StringLib.h"

const SoundSetDef* RaceDef::GetAdvisorError(AdvisorErrorMessageType type) const
{
	char path[32];

	stbsp_snprintf(path, 32, "Advisor\\%s\\Error0%i", Name.data(), (int)type);

	return GameDatabase::instance->GetSoundSet(path);
}

const SoundSetDef* RaceDef::GetAdvisorUpdate(AdvisorUpdateMessageType type) const
{
	char path[32];

	stbsp_snprintf(path, 32, "Advisor\\%s\\Update0%i", Name.data(), (int)type);

	return GameDatabase::instance->GetSoundSet(path);
}

const SoundSetDef* RaceDef::GetMusic() const
{
	char path[32];

	stbsp_snprintf(path, 32, "Music\\%s", Name.data());

	return GameDatabase::instance->GetSoundSet(path);
}
