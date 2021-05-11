#include "Archetype.h"
#include "Debug.h"
#include <cstring>

EntityArchetype::EntityArchetype(const char* name): name(name)
{
	memset(hasEntity.data(), false, Entity::MaxEntities * sizeof(bool));
}

void EntityArchetype::AddEntity(EntityId id)
{
	if (HasEntity(id))
		EXCEPTION("Entity %i already added to archetype %s!", name);

	hasEntity[Entity::ToIndex(id)] = true;
	newEntities.push_back(id);
	auto end = entities.crend();
	for (auto i = entities.crbegin(); i != end; ++i)
	{
		if (*i < id) {
			entities.insert(i.base(), id);
			return ;
		}
	}


	entities.insert(entities.begin(), id);
}

void EntityArchetype::RemoveEntity(EntityId id)
{
	if (!HasEntity(id))
		EXCEPTION("Entity %i is not part of archetype %s!", name);

	hasEntity[Entity::ToIndex(id)] = false;
	removedEntities.push_back(id);
	auto end = entities.cend();
	for (auto i = entities.cbegin(); i != end; ++i)
	{
		if (*i == id) {
			entities.erase(i);
			return;
		}
	}

	EXCEPTION("Deleting entity %id was not found in archetype %s!", id, name);
}

void EntityArchetype::CommitChanges()
{
	newEntities.clear();
	removedEntities.clear();
}
