#include "Archetype.h"
#include "Debug.h"
#include "../Profiler.h"

#include <algorithm>
#include <cstring>

EntityArchetype::EntityArchetype(const char* name) : name(name)
{
	memset(hasEntity.data(), false, Entity::MaxEntities * sizeof(bool));
}

void EntityArchetype::AddEntity(EntityId id)
{
	if (HasEntity(id))
		EXCEPTION("Entity %i already added to archetype %s!", id, name);

	hasEntity[Entity::ToIndex(id)] = true;
	newEntities.push_back(id);
	auto end = entities.crend();
	for (auto i = entities.crbegin(); i != end; ++i)
	{
		if (*i < id) {
			entities.insert(i.base(), id);
			return;
		}
	}

	entities.insert(entities.begin(), id);
}
void EntityArchetype::AddEntities(std::vector<EntityId>& add, bool sorted) {
	if (!sorted)
		std::sort(add.begin(), add.end());

	for (EntityId id : add)
		if (HasEntity(id)) {
			EXCEPTION("Entity %i is already part of archetype %s!", id, name);
		}
		else
			hasEntity[Entity::ToIndex(id)] = true;

	int insertAt = entities.size();
	EntityId id = add.front();
	auto sortEnd = insertAt;
	for (int i = 0; i < sortEnd; ++i)
	{
		if (entities[i] < id) {
			insertAt = i;
			break;
		}
	}

	entities.insert(entities.cbegin() + insertAt, add.cbegin(), add.cend());
	id = add.back();
	size_t end = std::min(entities.size(), insertAt + (size_t)id + 1);
	std::sort(entities.begin() + insertAt, entities.begin() + end);
}

void EntityArchetype::RemoveEntity(EntityId id)
{
	if (!HasEntity(id))
		EXCEPTION("Entity %i is not part of archetype %s!", id, name);

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

static const std::vector<EntityId>* s;
static  int iter = 0;

static bool RemoveIf(EntityId id) {
	auto& sratch = *s;
	if (iter < sratch.size() && sratch[iter] == id)
	{
		++iter;
		return true;
	}
	return false;

}

int EntityArchetype::RemoveEntitiesSorted(std::vector<EntityId>& del) {
	scratch.clear();

	for (EntityId id : del)
		if (!HasEntity(id))
			continue;
		else {
			scratch.push_back(id);
			hasEntity[Entity::ToIndex(id)] = false;
			removedEntities.push_back(id);
		}

	if (scratch.size() == 0)
		return 0;


	s = &scratch;
	iter = 0;


	entities.erase(std::remove_if(entities.begin(), entities.end(), RemoveIf), entities.end());

	return scratch.size();
}


int EntityArchetype::RemoveEntities( std::vector<EntityId>& del, bool sorted) {
	if (!sorted)
		std::sort(del.begin(), del.end());

	return RemoveEntitiesSorted(del);
}

void EntityArchetype::ClearEntities()
{
	entities.clear();
	newEntities.clear();
	removedEntities.clear();
	memset(hasEntity.data(), false, Entity::MaxEntities * sizeof(bool));
}

void EntityArchetype::CommitChanges()
{
	newEntities.clear();
	removedEntities.clear();
}
