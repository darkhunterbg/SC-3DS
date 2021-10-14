#include "ViewContext.h"
#include "../Entity/EntityUtil.h"
#include "../Entity/EntityManager.h"

ViewContext::ViewContext()
{
}

const PlayerInfo& ViewContext::GetPlayerInfo() const
{
	return EntityUtil::GetManager().PlayerSystem.GetPlayerInfo(_player);
}