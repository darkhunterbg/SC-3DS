#include "UnitPortraitDef.h"
#include "GameDatabase.h"

VideoClip& UnitPortraitDef::GetIdleClip(int index) const
{
	return GameDatabase::instance->GetVideoClip(IdleClipStart + index);
}

Span<VideoClip*> UnitPortraitDef::GetIdleClips() const
{
	return GameDatabase::instance->GetVideoClips(IdleClipStart, IdleClipCount);
}

VideoClip& UnitPortraitDef::GetActivatedClip(int index) const
{
	return GameDatabase::instance->GetVideoClip(ActivatedClipStart + index);
}

Span<VideoClip*> UnitPortraitDef::GetActivatedClips() const
{
	return GameDatabase::instance->GetVideoClips(ActivatedClipStart, ActivatedClipCount);
}
